#ifndef PARAMS_H
#define PARAMS_H

#include <string>

// Structure for solver parameters
struct SolverParams {
    // Time and solution limits
    int max_time = 300;           // Maximum time in seconds
    bool first_solution_only = false;  // Stop at first solution
    
    // Search strategies
    std::string var_strategy = "mrv";  // Variable selection strategy (mrv, degree, random)
    std::string val_strategy = "lcv";  // Value selection strategy (lcv, random, lexicographic)
    
    // Constraint propagation
    bool use_ac3 = true;          // Use AC-3
    bool use_forward_checking = true;  // Use forward checking
    bool ac3_at_each_node = true; // Apply AC-3 at each backtracking node
    
    // Output control
    bool verbose = false;         // Verbose mode (disabled by default)
    int max_depth_trace = 5;      // Maximum depth for detailed tracing
    int max_depth_ac3_trace = 3;  // Maximum depth for AC-3 tracing
    
    // Memory and performance
    bool show_memory_usage = true; // Show memory usage information
    bool show_global_stats_only = false; // Show main steps but not detailed tracing
    
    // Output path
    std::string output_path = "";      // Custom output path (empty = use default)
};

#endif // PARAMS_H
