#include "solution_writer.h"
#include "../parser/parser.h"
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <chrono>
#include "logo.h"

using namespace std;

// Format time in milliseconds
string formatTime(long milliseconds) {
    if (milliseconds < 1000) {
        return to_string(milliseconds) + "ms";
    } else if (milliseconds < 60000) {
        return to_string(milliseconds / 1000) + "." + 
               to_string((milliseconds % 1000) / 100) + "s";
    } else {
        long minutes = milliseconds / 60000;
        long seconds = (milliseconds % 60000) / 1000;
        return to_string(minutes) + "m" + to_string(seconds) + "s";
    }
}

// Get current timestamp
string getCurrentTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    stringstream ss;
    ss << setfill('0') << setw(4) << (1900 + ltm->tm_year) << "-"
       << setfill('0') << setw(2) << (1 + ltm->tm_mon) << "-"
       << setfill('0') << setw(2) << ltm->tm_mday << " "
       << setfill('0') << setw(2) << ltm->tm_hour << ":"
       << setfill('0') << setw(2) << ltm->tm_min << ":"
       << setfill('0') << setw(2) << ltm->tm_sec;
    
    return ss.str();
}

// Main function to write solutions
void writeSolutions(const string& filename, 
                   const vector<map<int, int>>& solutions,
                   const CSPInstance& csp,
                   const SolverParams& params,
                   long duration_ms,
                   int nodes_explored,
                   const string& resolution_status) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "ERROR: Cannot open solution file: " << filename << endl;
        return;
    }
    
    // Header
    printLogo(file);
    file << endl;
    
    // Metadata
    file << "# ┌─────────────────────────────────────────────────────────────────────────────┐" << endl;
    file << "# │                              SOLVER STATISTICS                              │" << endl;
    file << "# └─────────────────────────────────────────────────────────────────────────────┘" << endl;
    file << "# Generated on: " << getCurrentTimestamp() << endl;
    file << "# Variables: " << csp.num_variables << endl;
    file << "# Constraints: " << csp.constraints.size() << endl;
    file << "# Domain size: " << (csp.domains.empty() ? 0 : (csp.domains[0].second - csp.domains[0].first + 1)) << endl;
    file << "# Solutions found: " << solutions.size() << endl;
    file << "# Resolution status: " << resolution_status << endl;
    file << "# Nodes explored: " << nodes_explored << endl;
    file << "# Solving time: " << formatTime(duration_ms) << endl;
    file << "# Variable strategy: " << params.var_strategy << endl;
    file << "# Value strategy: " << params.val_strategy << endl;
    file << "# AC-3: " << (params.use_ac3 ? "Enabled" : "Disabled") << endl;
    file << "# Forward checking: " << (params.use_forward_checking ? "Enabled" : "Disabled") << endl;
    
    // Add verbose information if enabled
    if (params.verbose) {
        file << "#" << endl;
        file << "# ┌─────────────────────────────────────────────────────────────────────────────┐" << endl;
        file << "# │                              VERBOSE DETAILS                                │" << endl;
        file << "# └─────────────────────────────────────────────────────────────────────────────┘" << endl;
        file << "# Maximum time limit: " << params.max_time << " seconds" << endl;
        file << "# First solution only: " << (params.first_solution_only ? "Yes" : "No") << endl;
        file << "# AC-3 at each node: " << (params.ac3_at_each_node ? "Yes" : "No") << endl;
        file << "# Max depth for tracing: " << params.max_depth_trace << endl;
        file << "# Max depth for AC-3 tracing: " << params.max_depth_ac3_trace << endl;
        file << "#" << endl;
        file << "# Note: This solution file contains detailed solver information." << endl;
        file << "# All verbose output is prefixed with '#' to avoid parser conflicts." << endl;
    }
    
    file << "#" << endl;

    // Write solutions
    if (solutions.empty()) {
        file << "# No solution found" << endl;
    } else {
        for (size_t i = 0; i < solutions.size(); i++) {
            file << "# Solution " << (i + 1) << endl;
            const auto& solution = solutions[i];
            bool first = true;
            for (const auto& assignment : solution) {
                if (!first) file << " ";
                file << assignment.first << "=" << assignment.second;
                first = false;
            }
            file << endl;
        }
    }
    
    // Ensure all data is written and file is properly closed
    file.flush();
    file.close();
}