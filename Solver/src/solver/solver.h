#ifndef SOLVER_H
#define SOLVER_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "../parser/parser.h"

// Forward declaration
class SelectionStrategies;

// Main CSP solver class
class CSPSolver {
private:
    CSPInstance csp;
    std::vector<std::vector<int>> domains;  // Domaines pour chaque variable
    std::map<int, int> assignment;          // Assignation variable -> valeur
    std::vector<std::map<int, int>> solutions; // Solutions trouvées
    
    // Pre-computed static properties
    std::vector<std::vector<int>> var_interaction_graph;

    // Statistics
    int nodes_explored;
    int backtracks;
    bool timeout_occurred;
    std::chrono::high_resolution_clock::time_point start_time;
    
    // Private methods
    bool isComplete() const;
    bool isConsistent(int var, int value) const;
    bool forwardCheckWithDomainReduction(int var, int value);
    bool validateSolution(const std::map<int, int>& solution) const;
    void restoreDomains(const std::vector<std::vector<int>>& backup);
    std::vector<std::vector<int>> backupDomains() const;
    bool backtrack(int depth, const std::string& var_strategy, const std::string& val_strategy,
                  bool use_forward_checking, int max_time, bool first_solution_only,
                  bool verbose, SelectionStrategies& strategies,
                  bool ac3_at_each_node = true, int max_depth_trace = 5, 
                  int max_depth_ac3_trace = 3, bool show_global_stats_only = false);
    
public:
    CSPSolver(const CSPInstance& instance);
    
    // Main solving method
    bool solve(std::vector<std::map<int, int>>& solutions,
              int max_time,
              bool first_solution_only,
              const std::string& var_strategy,
              const std::string& val_strategy,
              bool use_forward_checking,
              bool verbose,
              bool ac3_at_each_node = true,
              int max_depth_trace = 5,
              int max_depth_ac3_trace = 3,
              bool show_global_stats_only = false);
    
    // Apply AC-3
    bool applyAC3(bool verbose = true);
    
    // Get statistics
    int getNodesExplored() const { return nodes_explored; }
    int getBacktracks() const { return backtracks; }
    bool wasTimeout() const { return timeout_occurred; }
    void printStats() const;
};

#endif // SOLVER_H
