
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <sys/wait.h>

using namespace std;

// Helper function to get local IP
string getLocalIP() {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    struct hostent* he = gethostbyname(hostname);
    if (!he || !he->h_addr_list[0]) {
        cerr << "Failed to get local IP" << endl;
        exit(1);
    }
    struct in_addr addr;
    memcpy(&addr, he->h_addr_list[0], sizeof(struct in_addr));
    return inet_ntoa(addr);
}

int main(int argc, char *argv[]) {
    string exec_cmd, agent_path, ssh_path, query;
    vector<string> remote_ips;
    int iterations = -1, port = -1;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-e" && i + 1 < argc) exec_cmd = argv[++i];
        else if (arg == "-a" && i + 1 < argc) agent_path = argv[++i];
        else if (arg == "-s" && i + 1 < argc) ssh_path = argv[++i];
        else if (arg == "-r" && i + 1 < argc) remote_ips.push_back(argv[++i]);
        else if (arg == "-q" && i + 1 < argc) query = argv[++i];
        else if (arg == "-n" && i + 1 < argc) iterations = stoi(argv[++i]);
        else if (arg == "-p" && i + 1 < argc) port = stoi(argv[++i]);
        else {
            cerr << "Invalid or missing argument: " << arg << endl;
            return 1;
        }
    }

    if (exec_cmd.empty() || agent_path.empty() || ssh_path.empty() || remote_ips.empty() ||
        query.empty() || iterations <= 0 || port <= 0) {
        cerr << "Missing required arguments." << endl;
        return 1;
    }

    // Get local IP
    string local_ip = getLocalIP();

    // Create TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

        int bind_result = ::bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (bind_result < 0) {
            perror("bind");
            return 1;
        }

    if (listen(server_fd, static_cast<int>(remote_ips.size())) < 0) {
        perror("listen");
        return 1;
    }

    // Launch agents
    for (const string& remote_ip : remote_ips) {
        pid_t pid = fork();
        if (pid == 0) {
            ostringstream cmd;
            // cmd << agent_path
            //     << " -e '" << exec_cmd << "'"
            //     << " -q " << query
            //     << " -i " << local_ip
            //     << " -p " << port;
            cmd << "'" << agent_path
                << " -e \"" << exec_cmd << "\""
                << " -q " << query
                << " -i " << local_ip
                << " -p " << port << "'";
            execlp(ssh_path.c_str(), ssh_path.c_str(), remote_ip.c_str(), "sh", "-c", cmd.str().c_str(), (char*)nullptr);
            exit(1);
        }
    }

    // Accept connections
    vector<int> client_sockets;
    for (size_t i = 0; i < remote_ips.size(); ++i) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
        if (client_fd >= 0) client_sockets.push_back(client_fd);
    }

    // Execute commands for N iterations
    for (int round = 1; round <= iterations; ++round) {
        cout << "N=" << round << endl;

        // Send EXECUTE
        for (int fd : client_sockets)
            send(fd, "EXECUTE", 8, 0);

        // Read responses in order of IPs
        for (size_t i = 0; i < remote_ips.size(); ++i) {
            int fd = client_sockets[i];
            string prefix = remote_ips[i] + "::";

            string line;
            char buffer[512];
            bool in_result = false;

            while (true) {
                ssize_t len = recv(fd, buffer, sizeof(buffer) - 1, 0);
                if (len <= 0) break;
                buffer[len] = '\0';
                istringstream iss(buffer);
                while (getline(iss, line)) {
                    if (line == "START") {
                        in_result = true;
                    } else if (line == "STOP") {
                        in_result = false;
                        break;
                    } else if (in_result) {
                        cout << prefix << line << endl;
                    }
                }
                if (!in_result) break;
            }
        }
    }

    // Send QUIT to all clients
    for (int fd : client_sockets) send(fd, "QUIT", 5, 0);
    for (int fd : client_sockets) close(fd);
    close(server_fd);
    return 0;
}
