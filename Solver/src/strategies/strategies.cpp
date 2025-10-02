#include "strategies.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <climits>
#include <limits>

using namespace std;

SelectionStrategies::SelectionStrategies(const CSPInstance& csp_instance, 
                                       const std::vector<std::vector<int>>& current_domains,
                                       const std::map<int, int>& current_assignment,
                                       const std::vector<std::vector<int>>& graph)
    : csp(csp_instance), 
      domains(current_domains), 
      assignment(current_assignment),
      var_interaction_graph(graph),
      rng(std::random_device{}()) {}

// Main function for variable selection
int SelectionStrategies::selectVariable(const string& strategy) const {
    if (strategy == "mrv") {
        return mrvHeuristic();
    } else if (strategy == "degree") {
        return degreeHeuristic();
    } else if (strategy == "random") {
        return randomVariable();
    }
    // Default to MRV
    return mrvHeuristic();
}

// Main function for value ordering
vector<int> SelectionStrategies::orderValues(int var, const string& strategy) const {
    if (strategy == "lcv") {
        return lcvHeuristic(var);
    } else if (strategy == "random") {
        return randomValues(var);
    } else if (strategy == "lexicographic") {
        return lexicographicValues(var);
    }
    // Default to lexicographic
    return lexicographicValues(var);
}


// --- Variable Selection Heuristics ---

int SelectionStrategies::mrvHeuristic() const {
    int selected_var = -1;
    size_t min_domain_size = std::numeric_limits<size_t>::max();

    for (int i = 0; i < csp.num_variables; i++) {
        if (assignment.find(i) == assignment.end()) { // Unassigned
            if (domains[i].size() < min_domain_size) {
                min_domain_size = domains[i].size();
                selected_var = i;
            }
        }
    }
    return selected_var;
}

int SelectionStrategies::degreeHeuristic() const {
    int selected_var = -1;
    int max_degree = -1;

    for (int i = 0; i < csp.num_variables; i++) {
        if (assignment.find(i) == assignment.end()) { // Unassigned
            int degree = var_interaction_graph[i].size();
            if (degree > max_degree) {
                max_degree = degree;
                selected_var = i;
            }
        }
    }
    return selected_var;
}

int SelectionStrategies::randomVariable() const {
    std::vector<int> unassigned_vars;
    for (int i = 0; i < csp.num_variables; i++) {
        if (assignment.find(i) == assignment.end()) {
            unassigned_vars.push_back(i);
        }
    }

    if (unassigned_vars.empty()) {
        return -1;
    }

    std::uniform_int_distribution<size_t> dist(0, unassigned_vars.size() - 1);
    return unassigned_vars[dist(rng)];
}


// --- Value Ordering Heuristics ---

vector<int> SelectionStrategies::lcvHeuristic(int var) const {
    std::vector<pair<int, int>> value_conflicts;
    for (int value : domains[var]) {
        value_conflicts.push_back({value, countConflicts(var, value)});
    }

    std::sort(value_conflicts.begin(), value_conflicts.end(), 
              [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second < b.second;
    });

    std::vector<int> sorted_values;
    for (const auto& pair : value_conflicts) {
        sorted_values.push_back(pair.first);
    }
    return sorted_values;
}

vector<int> SelectionStrategies::randomValues(int var) const {
    vector<int> values = domains[var];
    std::shuffle(values.begin(), values.end(), rng);
    return values;
}

vector<int> SelectionStrategies::lexicographicValues(int var) const {
    vector<int> values = domains[var];
    // The domains are already sorted from initialization, but let's be safe.
    std::sort(values.begin(), values.end());
    return values;
}


// --- LCV Helper ---

int SelectionStrategies::countConflicts(int var, int value) const {
    int conflicts = 0;
    for (int neighbor : var_interaction_graph[var]) {
        if (assignment.find(neighbor) == assignment.end()) { // Unassigned
            for (int neighbor_value : domains[neighbor]) {
                if (!csp.isConsistent(var, value, neighbor, neighbor_value)) {
                    conflicts++;
                }
            }
        }
    }
    return conflicts;
}