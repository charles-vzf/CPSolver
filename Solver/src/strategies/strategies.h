#ifndef STRATEGIES_H
#define STRATEGIES_H

#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include "../parser/parser.h"

// Class for variable and value selection strategies
class SelectionStrategies {
private:
    const CSPInstance& csp;
    const std::vector<std::vector<int>>& domains;
    const std::map<int, int>& assignment;
    const std::vector<std::vector<int>>& var_interaction_graph; // Constraint graph
    mutable std::mt19937 rng; // Mutable for random number generation in const methods
    
    // Heuristics for variable selection
    int mrvHeuristic() const;
    int degreeHeuristic() const;
    int randomVariable() const;
    
    // Heuristics for value ordering
    std::vector<int> lcvHeuristic(int var) const;
    std::vector<int> randomValues(int var) const;
    std::vector<int> lexicographicValues(int var) const;

    // For LCV
    int countConflicts(int var, int value) const;
    
public:
    SelectionStrategies(const CSPInstance& csp_instance, 
                        const std::vector<std::vector<int>>& current_domains,
                        const std::map<int, int>& current_assignment,
                        const std::vector<std::vector<int>>& graph);
    
    // Variable selection
    int selectVariable(const std::string& strategy) const;
    
    // Value ordering
    std::vector<int> orderValues(int var, const std::string& strategy) const;
};

#endif // STRATEGIES_H
