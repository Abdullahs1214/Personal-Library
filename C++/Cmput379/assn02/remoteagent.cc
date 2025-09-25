#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <map>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

struct ProcessInfo {
    int pid;
    int ppid;
    string cmd;
};

int main(int argc, char *argv[]) {
    string query_str;
    string exec_cmd;
    string ip;
    int port = -1;

    // === Argument Parsing ===
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-q" && i + 1 < argc) query_str = argv[++i];
        else if (arg == "-e" && i + 1 < argc) exec_cmd = argv[++i];
        else if (arg == "-i" && i + 1 < argc) ip = argv[++i];
        else if (arg == "-p" && i + 1 < argc) port = stoi(argv[++i]);
        else {
            cerr << "Missing or unknown argument: " << arg << endl;
            return 1;
        }
    }

    // === Socket Setup ===
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "Error creating socket." << endl;
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        cerr << "Invalid IP address." << endl;
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Connection failed." << endl;
        return 1;
    }

    cout << "✅ Connected: Hello from remoteagent" << endl;

    // === Command Loop ===
    char buffer[512];
    while (true) {
        ssize_t bytesReceived = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) break;

        buffer[bytesReceived] = '\0';
        string command(buffer);
        command.erase(0, command.find_first_not_of(" \t\r\n"));
        command.erase(command.find_last_not_of(" \t\r\n") + 1);

        if (command == "QUIT") {
            break;
        } else if (command == "EXECUTE") {
            FILE* pipe = popen(exec_cmd.c_str(), "r");
            if (!pipe) {
                cerr << "Failed to run execution command." << endl;
                continue;
            }

            send(sockfd, "START\n", 6, 0);

            map<int, ProcessInfo> processTable;
            char line[512];

            while (fgets(line, sizeof(line), pipe)) {
                istringstream iss(line);
                int pid, ppid;
                string user, args;

                if (!(iss >> pid >> ppid >> user)) continue;

                getline(iss, args);
                args.erase(0, args.find_first_not_of(" \t"));

                ProcessInfo info;
                info.pid = pid;
                info.ppid = ppid;
                info.cmd = args.substr(0, args.find(' '));
                processTable[pid] = info;

                if (args.find(query_str) != string::npos) {
                    string parentCmd = processTable.count(ppid) ? processTable[ppid].cmd : "UNKNOWN";
                    ostringstream out;
                    out << parentCmd << "(" << ppid << ") -- " << info.cmd << "(" << pid << ")\n";

                    string matchLine = out.str();
                    send(sockfd, matchLine.c_str(), matchLine.length(), 0);
                }
            }

            send(sockfd, "STOP\n", 5, 0);
            pclose(pipe);
        } else {
            string unknown = "❓ Unknown command: " + command + "\n";
            send(sockfd, unknown.c_str(), unknown.length(), 0);
        }
    }

    close(sockfd);
    return 0;
}
