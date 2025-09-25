#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_LINE 2048
#define MAX_ARGS 256
#define MAX_BG   256

/* background pid table */
static pid_t bg_table[MAX_BG];
static size_t bg_count = 0;

/* Add pid to bg_table */
static void bg_add(pid_t pid) {
  if (bg_count < MAX_BG) bg_table[bg_count++] = pid;
}

/* Remove pid from bg_table */
static void bg_remove(pid_t pid) {
  size_t i = 0;
  while (i < bg_count && bg_table[i] != pid) ++i;
  if (i == bg_count) return;
  for (; i + 1 < bg_count; ++i) bg_table[i] = bg_table[i+1];
  bg_count--;
}

/* Reap background pids */
static void reap_bg_children(void) {
  for (size_t i = 0; i < bg_count; ) {
    pid_t pid = bg_table[i];
    int status;
    pid_t r = waitpid(pid, &status, WNOHANG);
    if (r == 0) {
      ++i; /* still running */
      continue;
    } else if (r == -1) {
      if (errno == ECHILD || errno == ESRCH) {
        bg_remove(pid);
        continue;
      } else {
        ++i;
        continue;
      }
    } else {
      printf("[background pid %d terminated]\n", (int)pid);
      fflush(stdout);
      bg_remove(pid);
      continue;
    }
  }
}

/* SIGCHLD handler */
static void sigchld_handler(int signo) {
  (void)signo;
  int saved_errno = errno;
  reap_bg_children();
  errno = saved_errno;
}

/* ---------------- Tokenizer (allocating tokens) ----------------
   Allocates tokens (caller must free them later).
   - recognizes >>, >, <, |, & as tokens
   - treats single/double quoted strings as single tokens (quotes removed)
   - splits on whitespace otherwise
   - returns token count (>=0), argv is NULL-terminated
*/
static int tokenize_alloc(char *line, char **argv) {
  if (!line || !argv) return -1;
  char *p = line;
  int argc = 0;

  while (*p != '\0') {
    /* skip whitespace */
    while (*p == ' ' || *p == '\t') ++p;
    if (*p == '\0') break;
    if (argc >= MAX_ARGS - 1) break;

    /* >> */
    if (*p == '>' && *(p+1) == '>') {
      argv[argc] = malloc(3);
      if (!argv[argc]) { perror("malloc"); exit(1); }
      argv[argc][0] = '>'; argv[argc][1] = '>'; argv[argc][2] = '\0';
      ++argc;
      p += 2;
      continue;
    }

    /* single-char operators */
    if (*p == '>' || *p == '<' || *p == '|' || *p == '&') {
      argv[argc] = malloc(2);
      if (!argv[argc]) { perror("malloc"); exit(1); }
      argv[argc][0] = *p;
      argv[argc][1] = '\0';
      ++argc;
      ++p;
      continue;
    }

    /* quoted token */
    if (*p == '"' || *p == '\'') {
      char quote = *p++;
      char *start = p;
      /* find matching quote or end */
      while (*p != '\0' && *p != quote) ++p;
      size_t len = (size_t)(p - start);
      argv[argc] = malloc(len + 1);
      if (!argv[argc]) { perror("malloc"); exit(1); }
      if (len > 0) memcpy(argv[argc], start, len);
      argv[argc][len] = '\0';
      ++argc;
      if (*p == quote) ++p; /* skip closing quote */
      continue;
    }

    /* normal token (unquoted) */
    char *start = p;
    while (*p != '\0' && *p != ' ' && *p != '\t' &&
           *p != '<' && *p != '>' && *p != '|' && *p != '&' &&
           *p != '"' && *p != '\'') {
      ++p;
    }
    size_t len = (size_t)(p - start);
    argv[argc] = malloc(len + 1);
    if (!argv[argc]) { perror("malloc"); exit(1); }
    if (len > 0) memcpy(argv[argc], start, len);
    argv[argc][len] = '\0';
    ++argc;
    /* loop continues; p currently at whitespace or special char or end */
  }

  argv[argc] = NULL;
  return argc;
}

/* Free an allocated token array (count tokens) */
static void free_tokens(char **tokens, int count) {
  if (!tokens) return;
  for (int i = 0; i < count; ++i) {
    if (tokens[i]) free(tokens[i]);
    tokens[i] = NULL;
  }
  tokens[0] = NULL;
}

/* ---------------- small helpers (unchanged logic) ---------------- */

static void print_prompt(void) {
  printf("osh> ");
  fflush(stdout);
}

/* Find stdout redirection '>' or '>>' */
int handle_stdout_redirection(char **argv, int *mode, char **fname) {
  if (argv == NULL) return 0;
  for (int i = 0; argv[i] != NULL; ++i) {
    if (strcmp(argv[i], ">") == 0 || strcmp(argv[i], ">>") == 0) {
      if (argv[i+1] == NULL) return -1;
      *mode = (strcmp(argv[i], ">") == 0) ? 1 : 2;
      *fname = argv[i+1];
      argv[i] = NULL;
      return *mode;
    }
  }
  return 0;
}

/* Find stdin redirection '<' */
int handle_stdin_redirection(char **argv, char **fname) {
  if (argv == NULL) return 0;
  for (int i = 0; argv[i] != NULL; ++i) {
    if (strcmp(argv[i], "<") == 0) {
      if (argv[i+1] == NULL) return -1;
      *fname = argv[i+1];
      argv[i] = NULL;
      return 1;
    }
  }
  return 0;
}

/* Check trailing '&' for background execution; remove it if present */
int check_background(char **argv) {
  if (argv == NULL) return 0;
  int i = 0;
  while (argv[i] != NULL) ++i;
  if (i == 0) return 0;
  if (strcmp(argv[i-1], "&") == 0) {
    argv[i-1] = NULL;
    return 1;
  }
  return 0;
}

/* Split argv into left and right around first '|' token */
int split_on_pipe(char **argv, char **rightargv) {
  if (argv == NULL || rightargv == NULL) return 0;
  int i = 0;
  while (argv[i] != NULL) {
    if (strcmp(argv[i], "|") == 0) {
      if (argv[i+1] == NULL) return -1;  /* syntax error: no right command */
      argv[i] = NULL;
      int j = 0;
      while (argv[i+1] != NULL) rightargv[j++] = argv[++i];
      rightargv[j] = NULL;
      return 1;
    }
    ++i;
  }
  rightargv[0] = NULL;
  return 0;
}

int open_out_file(const char *fname, int mode) {
  if (mode == 1) return open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  else return open(fname, O_WRONLY | O_CREAT | O_APPEND, 0644);
}

/* ---------------- main loop ---------------- */

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  /* Install SIGCHLD handler */
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigchld_handler;
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  if (sigaction(SIGCHLD, &sa, NULL) < 0) {
    fprintf(stderr, "sigaction failed: %s\n", strerror(errno));
  }

  char line[MAX_LINE];
  char *argvv[MAX_ARGS];
  char *saved_allocs[MAX_ARGS];

  fprintf(stdout, "Simple shell (type 'exit' or Ctrl-D to quit)\n");

  while (1) {
    print_prompt();
    if (fgets(line, sizeof(line), stdin) == NULL) {
      if (feof(stdin)) { printf("\nEOF received. Exiting.\n"); break; }
      else { fprintf(stderr, "fgets error: %s\n", strerror(errno)); break; }
    }

    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
    if (line[0] == '\0') continue;

    /* Tokenize into allocated strings */
    int ntoks = tokenize_alloc(line, argvv);
    if (ntoks < 0) continue;

    /* Save pointers so we can free later even if argvv gets modified */
    for (int i = 0; i < ntoks; ++i) saved_allocs[i] = argvv[i];
    saved_allocs[ntoks] = NULL;

    if (argvv[0] == NULL) { free_tokens(saved_allocs, ntoks); continue; }
    if (strcmp(argvv[0], "exit") == 0) {
      free_tokens(saved_allocs, ntoks);
      break;
    }

    /* Background marker for entire command/pipeline */
    int background = check_background(argvv);

    /* Split on pipe */
    char *rightargv[MAX_ARGS];
    int pipe_present = split_on_pipe(argvv, rightargv);
    if (pipe_present == -1) {
      fprintf(stderr, "syntax error: '|' with no right-hand command\n");
      free_tokens(saved_allocs, ntoks);
      continue;
    }

    if (!pipe_present) {
      /* Single command */
      int redir_mode = 0;
      char *outfname = NULL;
      int rh = handle_stdout_redirection(argvv, &redir_mode, &outfname);
      if (rh == -1) {
        fprintf(stderr, "redirection error: missing filename after '>' or '>>'\n");
        free_tokens(saved_allocs, ntoks);
        continue;
      }
      char *infname = NULL;
      int inh = handle_stdin_redirection(argvv, &infname);
      if (inh == -1) {
        fprintf(stderr, "redirection error: missing filename after '<'\n");
        free_tokens(saved_allocs, ntoks);
        continue;
      }

      pid_t pid = fork();
      if (pid < 0) {
        fprintf(stderr, "fork failed: %s\n", strerror(errno));
        free_tokens(saved_allocs, ntoks);
        continue;
      } else if (pid == 0) {
        /* Child: handle redirections */
        if (inh == 1) {
          int fd = open(infname, O_RDONLY);
          if (fd < 0) { fprintf(stderr, "failed to open '%s' for input: %s\n", infname, strerror(errno)); _exit(EXIT_FAILURE); }
          if (dup2(fd, STDIN_FILENO) < 0) { fprintf(stderr, "dup2 failed: %s\n", strerror(errno)); close(fd); _exit(EXIT_FAILURE); }
          close(fd);
        }
        if (redir_mode != 0) {
          int fd = open_out_file(outfname, redir_mode);
          if (fd < 0) { fprintf(stderr, "failed to open '%s' for redir: %s\n", outfname, strerror(errno)); _exit(EXIT_FAILURE); }
          if (dup2(fd, STDOUT_FILENO) < 0) { fprintf(stderr, "dup2 failed: %s\n", strerror(errno)); close(fd); _exit(EXIT_FAILURE); }
          close(fd);
        }
        execvp(argvv[0], argvv);
        fprintf(stderr, "execvp failed for '%s': %s\n", argvv[0], strerror(errno));
        _exit(EXIT_FAILURE);
      } else {
        if (background) { bg_add(pid); printf("[background pid %d]\n", (int)pid); fflush(stdout); }
        else {
          int status;
          if (waitpid(pid, &status, 0) < 0) { if (errno != ECHILD) fprintf(stderr, "waitpid error: %s\n", strerror(errno)); }
        }
        /* free tokens allocated for this command */
        free_tokens(saved_allocs, ntoks);
      }
      continue;
    }

    /* PIPE present */
    int left_out_mode = 0; char *left_out_fname = NULL;
    int lh_out = handle_stdout_redirection(argvv, &left_out_mode, &left_out_fname);
    if (lh_out == -1) { fprintf(stderr, "redirection error: missing filename after '>' or '>>' on left\n"); free_tokens(saved_allocs, ntoks); continue; }
    char *left_in_fname = NULL; int lh_in = handle_stdin_redirection(argvv, &left_in_fname);
    if (lh_in == -1) { fprintf(stderr, "redirection error: missing filename after '<' on left\n"); free_tokens(saved_allocs, ntoks); continue; }

    int right_out_mode = 0; char *right_out_fname = NULL;
    int rh_out = handle_stdout_redirection(rightargv, &right_out_mode, &right_out_fname);
    if (rh_out == -1) { fprintf(stderr, "redirection error: missing filename after '>' or '>>' on right\n"); free_tokens(saved_allocs, ntoks); continue; }
    char *right_in_fname = NULL; int rh_in = handle_stdin_redirection(rightargv, &right_in_fname);
    if (rh_in == -1) { fprintf(stderr, "redirection error: missing filename after '<' on right\n"); free_tokens(saved_allocs, ntoks); continue; }

    int pipefd[2];
    if (pipe(pipefd) < 0) { fprintf(stderr, "pipe failed: %s\n", strerror(errno)); free_tokens(saved_allocs, ntoks); continue; }

    /* Left child */
    pid_t left_pid = fork();
    if (left_pid < 0) { fprintf(stderr, "fork failed for left: %s\n", strerror(errno)); close(pipefd[0]); close(pipefd[1]); free_tokens(saved_allocs, ntoks); continue; }
    else if (left_pid == 0) {
      if (lh_in == 1) {
        int fd = open(left_in_fname, O_RDONLY);
        if (fd < 0) { fprintf(stderr, "failed to open '%s' for left input: %s\n", left_in_fname, strerror(errno)); _exit(EXIT_FAILURE); }
        if (dup2(fd, STDIN_FILENO) < 0) { fprintf(stderr, "dup2 failed: %s\n", strerror(errno)); close(fd); _exit(EXIT_FAILURE); }
        close(fd);
      }
      if (left_out_mode != 0) {
        int fd = open_out_file(left_out_fname, left_out_mode);
        if (fd < 0) { fprintf(stderr, "failed to open '%s' for left redirection: %s\n", left_out_fname, strerror(errno)); _exit(EXIT_FAILURE); }
        if (dup2(fd, STDOUT_FILENO) < 0) { fprintf(stderr, "dup2 failed: %s\n", strerror(errno)); close(fd); _exit(EXIT_FAILURE); }
        close(fd);
      } else {
        if (dup2(pipefd[1], STDOUT_FILENO) < 0) { fprintf(stderr, "dup2(pipe write) failed: %s\n", strerror(errno)); _exit(EXIT_FAILURE); }
      }
      close(pipefd[0]); close(pipefd[1]);
      execvp(argvv[0], argvv);
      fprintf(stderr, "execvp failed for left '%s': %s\n", argvv[0], strerror(errno));
      _exit(EXIT_FAILURE);
    }

    /* Right child */
    pid_t right_pid = fork();
    if (right_pid < 0) { fprintf(stderr, "fork failed for right: %s\n", strerror(errno)); close(pipefd[0]); close(pipefd[1]); free_tokens(saved_allocs, ntoks); continue; }
    else if (right_pid == 0) {
      if (right_out_mode != 0) {
        int fd = open_out_file(right_out_fname, right_out_mode);
        if (fd < 0) { fprintf(stderr, "failed to open '%s' for right redirection: %s\n", right_out_fname, strerror(errno)); _exit(EXIT_FAILURE); }
        if (dup2(fd, STDOUT_FILENO) < 0) { fprintf(stderr, "dup2 failed: %s\n", strerror(errno)); close(fd); _exit(EXIT_FAILURE); }
        close(fd);
      }
      if (rh_in == 1) {
        int fd = open(right_in_fname, O_RDONLY);
        if (fd < 0) { fprintf(stderr, "failed to open '%s' for right input: %s\n", right_in_fname, strerror(errno)); _exit(EXIT_FAILURE); }
        if (dup2(fd, STDIN_FILENO) < 0) { fprintf(stderr, "dup2 failed: %s\n", strerror(errno)); close(fd); _exit(EXIT_FAILURE); }
        close(fd);
      } else {
        if (dup2(pipefd[0], STDIN_FILENO) < 0) { fprintf(stderr, "dup2(pipe read) failed: %s\n", strerror(errno)); _exit(EXIT_FAILURE); }
      }
      close(pipefd[0]); close(pipefd[1]);
      execvp(rightargv[0], rightargv);
      fprintf(stderr, "execvp failed for right '%s': %s\n", rightargv[0], strerror(errno));
      _exit(EXIT_FAILURE);
    }

    /* Parent */
    close(pipefd[0]); close(pipefd[1]);
    if (background) { bg_add(left_pid); bg_add(right_pid); printf("[background pids %d %d]\n", (int)left_pid, (int)right_pid); fflush(stdout); }
    else {
      int status;
      if (waitpid(left_pid, &status, 0) < 0) if (errno != ECHILD) fprintf(stderr, "waitpid error (left): %s\n", strerror(errno));
      if (waitpid(right_pid, &status, 0) < 0) if (errno != ECHILD) fprintf(stderr, "waitpid error (right): %s\n", strerror(errno));
    }

    /* free tokens allocated for this command/pipeline */
    free_tokens(saved_allocs, ntoks);

  } /* while */

  return 0;
}
