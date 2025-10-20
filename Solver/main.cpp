#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <cstdlib> // For system()
// #include <sys/resource.h> // For memory usage

// Custom headers
#include "src/parser/parser.h"
#include "src/solver/solver.h"
#include "src/algorithms/ac3.h"
#include "src/strategies/strategies.h"
#include "src/io/solution_writer.h"
#include "src/core/params.h"
#include "src/io/logo.h"

using namespace std;

// Function to get current memory usage in MB
/*
long getMemoryUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss / 1024; // Convert to MB
}
*/

// Function to count lines in a file
int countFileLines(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return -1;
    
    int lines = 0;
    string line;
    while (getline(file, line)) {
        lines++;
    }
    file.close();
    return lines;
}

// Function to display help
void printHelp() {
    cout << "Usage: " << endl;
    cout << "  CPSolver <file.csp> [options]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -t <time>      Maximum solving time in seconds (default: 300)" << endl;
    cout << "  -f             Stop at first solution found" << endl;
    cout << "  -v <strategy>  Variable selection strategy: mrv, degree, random (default: mrv)" << endl;
    cout << "  -w <strategy>  Value selection strategy: lcv, random, lexicographic (default: lcv)" << endl;
    cout << "  -a             Disable AC-3" << endl;
    cout << "  -c             Disable forward checking" << endl;
    cout << "  -n             Disable AC-3 at each backtracking node" << endl;
    cout << "  -o <path>      Output file path (default: ../solutions/solutions/<filename>.sol)" << endl;
    cout << "  -V             Verbose mode (show detailed tracing)" << endl;
    cout << "  -h             Show this help" << endl;
    cout << endl;
    cout << "Input file format (.csp) - DIMACS CSP format:" << endl;
    cout << "  # Comments start with #" << endl;
    cout << "  n                    # Number of variables" << endl;
    cout << "  # Variable domains (variable_id min_value max_value)" << endl;
    cout << "  0 min_0 max_0" << endl;
    cout << "  1 min_1 max_1" << endl;
    cout << "  ..." << endl;
    cout << "  m                    # Number of constraints" << endl;
    cout << "  # Constraints (var1 var2 (value1,value2) (value3,value4) ...)" << endl;
    cout << "  var1 var2 (val1,val2) (val3,val4) ..." << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << "  CPSolver instance.csp" << endl;
    cout << "  CPSolver instance.csp -t 60 -f" << endl;
    cout << "  CPSolver instance.csp -v degree -w random" << endl;
    cout << "  CPSolver instance.csp -o my_solution.sol" << endl;
    cout << "  CPSolver instance.csp -V" << endl;
}

// Function to parse command line arguments
SolverParams parseArguments(int argc, char* argv[], bool& B) {
    SolverParams params;
    
    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "-t" && i + 1 < argc) {
            params.max_time = stoi(argv[++i]);
        } else if (arg == "-f") {
            params.first_solution_only = true;
        } else if (arg == "-v" && i + 1 < argc) {
            params.var_strategy = argv[++i];
        } else if (arg == "-w" && i + 1 < argc) {
            params.val_strategy = argv[++i];
        } else if (arg == "-a") {
            params.use_ac3 = false;
        } else if (arg == "-c") {
            params.use_forward_checking = false;
        } else if (arg == "-n") {
            params.ac3_at_each_node = false;
        } else if (arg == "-o" && i + 1 < argc) {
            params.output_path = argv[++i];
        } else if (arg == "-V") {
            params.verbose = true;
            params.show_global_stats_only = false;
        } else if (arg == "-h") {
            printHelp();
            B = true;
        } else {
            cerr << "Unknown option: " << arg << endl;
            printHelp();
            B = true;
        }
    }
    
    return params;
}

int main(int argc, char* argv[]) {
    printLogo(cout);
    // Check arguments
    if (argc < 2) {
        cerr << "ERROR: Insufficient arguments" << endl;
        printHelp();
        return 1;
    }
    
    // Check if first argument is help
    if (string(argv[1]) == "-h") {
        printHelp();
        return 0;
    }
    
    string filename = argv[1];
    
    // Check file existence
    ifstream file(filename);
    if (!file.good()) {
        cerr << "ERROR: Cannot open file '" << filename << "'" << endl;
        return 1;
    }
    file.close();
    
    // Parse parameters
    bool B = false;
    SolverParams params = parseArguments(argc, argv, B);
    if(B) return 1;
    
    // --- Configuration logic ---
    // If AC-3 is globally disabled, also disable it at each node
    if (!params.use_ac3) {
        params.ac3_at_each_node = false;
    }

    // --- Initial parsing and setup ---
    CSPInstance csp;
    string resolution_status = "Unknown";
    bool parsing_ok = false;
    try {
        cout << "┌─────────────────────────────────────────────────────────────────────────────┐" << endl;
        cout << "│                       PARSING CSP  & INITIALIZING SOLVER                    │" << endl;
        cout << "└─────────────────────────────────────────────────────────────────────────────┘" << endl;
        cout << "Parsing CSP file..." << endl;
        
        csp = parseCSPFile(filename);
        parsing_ok = true;

        if (params.verbose) {
            cout << "   Variables: " << csp.num_variables << endl;
            cout << "   Constraints: " << csp.constraints.size() << endl;
        } else {
            cout << "   Variables: " << csp.num_variables << ", Constraints: " << csp.constraints.size() << endl;
        }
        cout << endl;

    } catch (const exception& e) {
        cerr << "ERROR during parsing: " << e.what() << endl;
        resolution_status = "Parsing Error";
        // We will proceed to write an empty solution file
    }

    // --- Solver execution ---
    vector<map<int, int>> solutions;
    long long solve_duration = 0;
    int nodes_explored = 0;
    int backtracks = 0;

    if (parsing_ok) {
        cout << "Initializing solver..." << endl;
        CSPSolver solver(csp);
        
        // Apply AC-3 if requested
        if (params.use_ac3) {
            cout << "Applying AC-3..." << endl;
            if (!solver.applyAC3(params.verbose)) {
                cout << "   Inconsistent instance detected by AC-3" << endl;
                resolution_status = "Inconsistent (AC-3)";
            }
            cout << "   AC-3 completed successfully" << endl << endl;
        }
        
        if (resolution_status == "Unknown") { // Proceed only if consistent so far
            // Backtracking resolution
            cout << "┌─────────────────────────────────────────────────────────────────────────────┐" << endl;
            cout << "│                           BACKTRACKING SEARCH                               │" << endl;
            cout << "└─────────────────────────────────────────────────────────────────────────────┘" << endl;
            cout << "Starting backtracking resolution..." << endl;
            
            auto start_time = chrono::high_resolution_clock::now();
            
            bool success = solver.solve(
                solutions,
                params.max_time,
                params.first_solution_only,
                params.var_strategy,
                params.val_strategy,
                params.use_forward_checking,
                params.verbose,
                params.ac3_at_each_node,
                params.max_depth_trace,
                params.max_depth_ac3_trace,
                params.show_global_stats_only
            );
            
            auto end_time = chrono::high_resolution_clock::now();
            solve_duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
            nodes_explored = solver.getNodesExplored();
            backtracks = solver.getBacktracks();

            // Determine resolution status
            if (success) {
                if (solutions.empty()) {
                    resolution_status = "No solution found";
                } else if (params.first_solution_only) {
                    resolution_status = "First solution found";
                } else {
                    resolution_status = "All solutions found";
                }
            } else {
                if (solver.wasTimeout()) {
                    resolution_status = "Timeout";
                } else {
                    resolution_status = "No solution (full exploration)";
                }
            }
        }
    }
    
    // --- Display results ---
    cout << endl;
    cout << "┌─────────────────────────────────────────────────────────────────────────────┐" << endl;
    cout << "│                                RESULTS                                      │" << endl;
    cout << "└─────────────────────────────────────────────────────────────────────────────┘" << endl;
    
    cout << "Resolution status: " << resolution_status << endl;
    cout << "Solutions found: " << solutions.size() << endl;
    cout << "Solving time: " << solve_duration << "ms" << endl;
    cout << "Nodes explored: " << nodes_explored << endl;
    cout << "Backtracks: " << backtracks << endl;
    
    if (!solutions.empty()) {
        cout << endl << "Solutions:" << endl;
        for (size_t i = 0; i < solutions.size(); i++) {
            cout << "Solution " << (i + 1) << ": ";
            for (const auto& assignment : solutions[i]) {
                cout << assignment.first << "=" << assignment.second << " ";
            }
            cout << endl;
        }
    }
    
    // --- Write solutions to file ---
    cout << endl;
    cout << "┌─────────────────────────────────────────────────────────────────────────────┐" << endl;
    cout << "│                            WRITING SOLUTIONS                                │" << endl;
    cout << "└─────────────────────────────────────────────────────────────────────────────┘" << endl;
    cout << "Writing solutions..." << endl;
    
    string output_file;
    if (!params.output_path.empty()) {
        output_file = params.output_path;
    } else {
        string base_filename = filename.substr(filename.find_last_of("/") + 1);
        string output_filename = base_filename.substr(0, base_filename.find_last_of(".")) + ".sol";
        output_file = "../solutions/solutions/" + output_filename; // Simplified path
        system("mkdir -p ../solutions/solutions");
    }
    
    writeSolutions(output_file, solutions, csp, params, solve_duration, nodes_explored, resolution_status);
    cerr << "Solutions saved to: " << output_file << endl;
    
    return 0;
}