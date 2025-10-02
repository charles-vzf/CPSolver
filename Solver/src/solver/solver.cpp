#include "solver.h"
#include "../algorithms/ac3.h"
#include "../strategies/strategies.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cassert>

using namespace std;

CSPSolver::CSPSolver(const CSPInstance& instance) 
    : csp(instance), nodes_explored(0), backtracks(0), timeout_occurred(false) {
    
    // Initialize domains from CSP instance
    domains.resize(csp.num_variables);
    for (int i = 0; i < csp.num_variables; i++) {
        int min_val = csp.domains[i].first;
        int max_val = csp.domains[i].second;
        for (int val = min_val; val <= max_val; val++) {
            domains[i].push_back(val);
        }
    }
    
    // Pre-compute static properties
    var_interaction_graph.resize(csp.num_variables);
    for (const auto& c : csp.constraints) {
        var_interaction_graph[c.var1].push_back(c.var2);
        var_interaction_graph[c.var2].push_back(c.var1);
    }

    // Make neighbor lists unique and sorted
    for (auto& neighbors : var_interaction_graph) {
        std::sort(neighbors.begin(), neighbors.end());
        neighbors.erase(std::unique(neighbors.begin(), neighbors.end()), neighbors.end());
    }
    
    assignment.clear();
    solutions.clear();
}

bool CSPSolver::isComplete() const {
    return static_cast<int>(assignment.size()) == csp.num_variables;
}

bool CSPSolver::isConsistent(int var, int value) const {
    for (const auto& constraint : csp.constraints) {
        if (constraint.var1 == var) {
            if (assignment.find(constraint.var2) != assignment.end()) {
                if (!csp.isConsistent(var, value, constraint.var2, assignment.at(constraint.var2))) {
                    return false;
                }
            }
        } else if (constraint.var2 == var) {
            if (assignment.find(constraint.var1) != assignment.end()) {
                if (!csp.isConsistent(constraint.var1, assignment.at(constraint.var1), var, value)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool CSPSolver::forwardCheckWithDomainReduction(int var, int value) {
    // Perform forward checking with actual domain reduction
    for (int neighbor : var_interaction_graph[var]) {
        if (assignment.find(neighbor) == assignment.end()) { // Unassigned neighbor
            std::vector<int> new_domain;
            for (int neighbor_value : domains[neighbor]) {
                if (csp.isConsistent(var, value, neighbor, neighbor_value)) {
                    new_domain.push_back(neighbor_value);
                }
            }
            
            if (new_domain.empty()) {
                // This assignment would make neighbor's domain empty
                // No need to restore domains here, as the backup is handled in backtrack
                return false;
            }
            
            domains[neighbor] = new_domain; // Update domain
        }
    }
    
    return true;
}

// Function to validate a complete solution
bool CSPSolver::validateSolution(const std::map<int, int>& solution) const {
    // Check that all variables are assigned
    if (static_cast<int>(solution.size()) != csp.num_variables) {
        std::cerr << "ERROR: Incomplete solution - " << solution.size() 
                  << " variables assigned out of " << csp.num_variables << std::endl;
        return false;
    }
    
    // Check that all values are within domains
    for (const auto& assignment : solution) {
        int var = assignment.first;
        int value = assignment.second;
        
        if (var < 0 || var >= csp.num_variables) {
            std::cerr << "ERROR: Variable " << var << " does not exist" << std::endl;
            return false;
        }
        
        int min_val = csp.domains[var].first;
        int max_val = csp.domains[var].second;
        if (value < min_val || value > max_val) {
            std::cerr << "ERROR: Variable " << var << " = " << value 
                      << " is outside of domain [" << min_val << ", " << max_val << "]" << std::endl;
            return false;
        }
    }
    
    // Check all constraints
    for (const auto& constraint : csp.constraints) {
        int var1 = constraint.var1;
        int var2 = constraint.var2;
        
        if (solution.count(var1) && solution.count(var2)) {
            int val1 = solution.at(var1);
            int val2 = solution.at(var2);
            
            if (!csp.isConsistent(var1, val1, var2, val2)) {
                std::cerr << "ERROR: Constraint violated between variables " << var1 
                          << "=" << val1 << " and " << var2 << "=" << val2 << std::endl;
                std::cerr << "Allowed pairs: ";
                for (const auto& pair : constraint.allowed_pairs) {
                    std::cerr << "(" << pair.first << "," << pair.second << ") ";
                }
                std::cerr << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

vector<vector<int>> CSPSolver::backupDomains() const {
    return domains;
}

void CSPSolver::restoreDomains(const vector<vector<int>>& backup) {
    domains = backup;
}

bool CSPSolver::applyAC3(bool verbose) {
    AC3Algorithm ac3(csp);
    ac3.setDomains(domains); // Initialize AC-3 with the current solver domains
    bool consistent = ac3.apply(verbose);
    
    if (consistent) {
        // Use AC3 domains directly
        domains = ac3.getDomains();
    }
    
    return consistent;
}

bool CSPSolver::solve(vector<map<int, int>>& solutions,
                     int max_time,
                     bool first_solution_only,
                     const string& var_strategy,
                     const string& val_strategy,
                     bool use_forward_checking,
                     bool verbose,
                     bool ac3_at_each_node,
                     int max_depth_trace,
                     int max_depth_ac3_trace,
                     bool show_global_stats_only) {
    
    start_time = chrono::high_resolution_clock::now();
    this->solutions.clear();
    nodes_explored = 0;
    backtracks = 0;
    timeout_occurred = false;
    
    SelectionStrategies strategies(csp, domains, assignment, var_interaction_graph);
    
    // Run backtracking search
    backtrack(0, var_strategy, val_strategy, use_forward_checking, 
              max_time, first_solution_only, verbose, strategies,
              ac3_at_each_node, max_depth_trace, max_depth_ac3_trace, show_global_stats_only);
    
    // Copy solutions back to the reference parameter
    solutions = this->solutions;
    
    // Return true if we found at least one solution
    return !solutions.empty();
}

bool CSPSolver::backtrack(int depth, const string& var_strategy, const string& val_strategy,
                         bool use_forward_checking, int max_time, bool first_solution_only,
                         bool verbose, SelectionStrategies& strategies,
                         bool ac3_at_each_node, int max_depth_trace, 
                         int max_depth_ac3_trace, bool show_global_stats_only) {
    
    // Check time limit
    auto current_time = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(current_time - start_time);
    if (elapsed.count() >= max_time) {
        timeout_occurred = true;
        if (verbose && !show_global_stats_only) cout << "   Time limit reached at depth " << depth << endl;
        return false;
    }
    
    // Check if complete
    if (isComplete()) {
        // Validate the solution with an assert
        assert(validateSolution(assignment) && "CRITICAL ERROR: Invalid solution found!");

        solutions.push_back(assignment);
        if (verbose && !show_global_stats_only) {
            cout << "   Solution found at depth " << depth << " (nodes: " << nodes_explored << ")" << endl;
        }
        // If we only want the first solution, return true to stop
        if (first_solution_only) {
            return true;
        }
        // Otherwise, continue searching for more solutions by returning false
        // This will make the parent function continue with the next value
        return false;
    }
    
    // --- Domain Backup ---
    // Backup domains before applying any filtering (AC-3 or FC)
    auto domain_backup = backupDomains();

    // Apply AC-3 at each node for domain filtering
    if (ac3_at_each_node) {
        if (!applyAC3(verbose && !show_global_stats_only && depth < max_depth_ac3_trace)) {
            // Inconsistent subset, backtrack
            restoreDomains(domain_backup); // Restore before returning
            return false; 
        }
    }
    
    // Select variable
    int var = strategies.selectVariable(var_strategy);
    
    // If var is -1, it means all variables are assigned. This should be caught by isComplete().
    assert(var != -1 || isComplete());
    if (var == -1) {
        return false; 
    }
    assert(assignment.find(var) == assignment.end() && "Selected variable is already assigned!");

    if (verbose && !show_global_stats_only && depth < max_depth_trace) {
        cout << "   Depth " << depth << ": selecting variable " << var 
             << " (domain size: " << domains[var].size() << ")" << endl;
        cout << "     Current domains: ";
        for (int i = 0; i < csp.num_variables; i++) {
            if (assignment.find(i) == assignment.end()) {
                cout << i << "[" << domains[i].size() << "] ";
            }
        }
        cout << endl;
    }
    
    // Order values for selected variable
    vector<int> values = strategies.orderValues(var, val_strategy);
    
    for (int value : values) {
        nodes_explored++;
        
        // Check consistency
        if (!isConsistent(var, value)) {
            continue;
        }
        
        // --- Forward Checking with Domain Reduction ---
        // Backup domains before applying forward checking
        vector<vector<int>> domain_backup_fc;
        if (use_forward_checking) {
            domain_backup_fc = backupDomains();
            if (!forwardCheckWithDomainReduction(var, value)) {
                // If FC fails, restore domains and prune this value
                restoreDomains(domain_backup_fc);
                continue;
            }
        }
        
        // Assign value
        assignment[var] = value;
        
        if (verbose && !show_global_stats_only && depth < max_depth_trace) {
            cout << "     Trying " << var << " = " << value << endl;
        }
        
        // Recursive call
        bool result = backtrack(depth + 1, var_strategy, val_strategy, 
                              use_forward_checking, max_time, first_solution_only, 
                              verbose, strategies, ac3_at_each_node, max_depth_trace, 
                              max_depth_ac3_trace, show_global_stats_only);
        
        // If we found a solution and only want the first one, return immediately
        if (result && first_solution_only) {
            return true;
        }
        
        // If we want all solutions, continue searching regardless of result
        // (result will be false when we continue searching after finding a solution)
        
        // --- Backtrack ---
        // Restore domains after trying a value
        if (use_forward_checking) {
            restoreDomains(domain_backup_fc);
        }

        // Backtrack
        assignment.erase(var);
        assert(assignment.find(var) == assignment.end() && "Backtrack failed to erase variable from assignment");
        backtracks++;
    }
    
    // --- Domain Restoration ---
    // Restore domains to their state before this variable's assignment
    restoreDomains(domain_backup);
    
    // Return false to continue searching for more solutions
    // The solutions are already stored in the solutions vector
    return false;
}

void CSPSolver::printStats() const {
    cout << "   Backtracking Statistics:" << endl;
    cout << "     Nodes explored: " << nodes_explored << endl;
    cout << "     Backtracks: " << backtracks << endl;
    cout << "     Solutions found: " << solutions.size() << endl;
}