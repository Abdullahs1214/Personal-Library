#!/usr/bin/env bash
# test_dragon.sh - quick non-interactive tests for dragonshell
# Save in same folder as 'dragonshell' binary

set -e

BIN=./dragonshell
TMPDIR=./_dragon_test_tmp
rm -rf "$TMPDIR"
mkdir -p "$TMPDIR"
cd "$TMPDIR"

echo "Starting tests..."

# 1) Basic echo and pipe with double quotes
echo 'echo "hello world" | tr a-z A-Z' | $BIN | tee t1.out
grep -q "HELLO WORLD" t1.out && echo "t1 OK" || { echo "t1 FAIL"; cat t1.out; exit 1; }

# 2) sed with single quotes and output redirection
# run non-interactively: feed the whole pipeline to shell and then check the out file
printf "echo line1 | sed 's/line/LINE/' > pipe_out.txt\nexit\n" | $BIN
if [ -f pipe_out.txt ]; then
  grep -q "^LINE1$" pipe_out.txt && echo "t2 OK" || { echo "t2 FAIL"; cat pipe_out.txt; exit 1; }
else
  echo "t2 FAIL - pipe_out.txt not created"; exit 1
fi

# 3) stdin redirection with cat and grep
printf "printf \"one\n two\n\" > in.txt\ncat < in.txt | grep two > found.txt\nexit\n" | $BIN
grep -q "two" found.txt && echo "t3 OK" || { echo "t3 FAIL"; exit 1; }

# 4) background job: simple sleep and immediate prompt return (we capture its printed termination)
# We run sleep in background and then wait a little to let it finish
printf "sleep 1 &\nexit\n" | $BIN > bg_test.out &
# give the background process time to run and print termination message
sleep 2
grep -q "background pid" bg_test.out && echo "t4 OK (background started)" || echo "t4 WARNING (background message missing)"

# 5) append redirection
printf "echo first > combined.txt\necho second >> combined.txt\ncat combined.txt\nexit\n" | $BIN | tee t5.out
grep -q "first" combined.txt && grep -q "second" combined.txt && echo "t5 OK" || { echo "t5 FAIL"; exit 1; }

# 6) mixing pipe and file redirection (left redir)
printf "echo foo > left.txt | cat\nexit\n" | $BIN
grep -q "^foo$" left.txt && echo "t6 OK" || { echo "t6 FAIL"; exit 1; }

# 7) quoting edge-case
printf "printf \"AAA\\n\" > rightin.txt\necho BBB | cat < rightin.txt\nexit\n" | $BIN | tee t7.out
grep -q "^AAA$" t7.out && echo "t7 OK" || { echo "t7 FAIL"; cat t7.out; exit 1; }

echo "All tests finished. Check outputs above."

cd ..
# keep the tmp dir for inspection
echo "Logs are in $TMPDIR"
