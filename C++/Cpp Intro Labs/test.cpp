#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

struct Process {
    int pid;
    int ppid;
    std::string user;
    std::string command;
};

std::vector<Process> processes; // Stores all processes
std::unordered_map<int, std::string> parent_map; // Stores PID -> Command mapping

// Function to parse process information
void parseLine(const std::string &line) {
    std::istringstream iss(line);
    Process p;
    iss >> p.pid >> p.ppid >> p.user;

    std::getline(iss >> std::ws, p.command);
    p.command.erase(0, p.command.find_first_not_of(" \t")); // Trim leading spaces
    p.command.erase(p.command.find_last_not_of(" \t") + 1); // Trim trailing spaces

    processes.push_back(p);

    

    //  Ensure parent process exists in `parent_map`, even if it's unknown
    if (parent_map.find(p.ppid) == parent_map.end()) {
        parent_map[p.ppid] = "Unknown";  // Store a placeholder if parent not seen yet
    }

    //  Store only meaningful parent processes
    if (!p.command.empty() && p.command.find('/') != std::string::npos) {
        if (parent_map.find(p.pid) == parent_map.end()) {
            parent_map[p.pid] = p.command;
        }
    }
}







// Function to display parent-child relationships
void filterProcesses(const std::string &query) {
    for (const auto &p : processes) {
        if (p.command.find(query) != std::string::npos) {
            std::string parentCommand = (parent_map.find(p.ppid) != parent_map.end()) ? parent_map[p.ppid] : "Unknown";
        if (parentCommand == "Unknown") {
            std::cout << "[WARNING] Parent not found for PPID: " << p.ppid << "\n";
        }

           

            std::cout << parentCommand << "(" << p.ppid << ") -- " 
                      << p.command << "(" << p.pid << ")\n";
        }
    }
}




int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " -e \"<command>\" \"<query_string>\"\n";
        return EXIT_FAILURE;
    }

    if (std::string(argv[1]) != "-e") {
        std::cerr << "Error: First argument must be '-e'\n";
        return EXIT_FAILURE;
    }

    std::string command = argv[2];
    std::string query = argv[3];

    std::cout << "Command: " << command << std::endl;
    std::cout << "Query: " << query << std::endl;

    FILE *fp = popen(command.c_str(), "r");
    if (!fp) {
        std::cerr << "Error: popen() failed\n";
        return EXIT_FAILURE;
    }

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), fp)) {
        std::cout << "Read: " << buffer;
        parseLine(buffer); // Store each process
    }

    pclose(fp);
    filterProcesses(query); // Display results
}
