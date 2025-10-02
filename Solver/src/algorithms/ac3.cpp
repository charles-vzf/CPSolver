#include "ac3.h"
#include "../parser/parser.h"
#include <algorithm>
#include <iostream>
#include <cassert>

using namespace std;

AC3Algorithm::AC3Algorithm(const CSPInstance& instance) 
    : csp(instance), revisions_count(0) {
    
    // Initialize domains from CSP instance
    domains.resize(csp.num_variables);
    for (int i = 0; i < csp.num_variables; i++) {
        int min_val = csp.domains[i].first;
        int max_val = csp.domains[i].second;
        for (int val = min_val; val <= max_val; val++) {
            domains[i].push_back(val);
        }
    }
    
    // Initialize worklist with all arcs
    for (const auto& constraint : csp.constraints) {
        worklist.push(Arc(constraint.var1, constraint.var2));
        worklist.push(Arc(constraint.var2, constraint.var1));
    }
}

bool AC3Algorithm::revise(int var1, int var2) {
    assert(var1 >= 0 && var1 < csp.num_variables && "revise(): var1 is out of bounds");
    assert(var2 >= 0 && var2 < csp.num_variables && "revise(): var2 is out of bounds");
    bool revised = false;
    vector<int> new_domain;
    
    for (int val1 : domains[var1]) {
        bool has_support = false;
        
        for (int val2 : domains[var2]) {
            if (isConsistent(var1, val1, var2, val2)) {
                has_support = true;
                break;
            }
        }
        
        if (has_support) {
            new_domain.push_back(val1);
        } else {
            revised = true;
        }
    }
    
    if (revised) {
        domains[var1] = new_domain;
        revisions_count++;
    }
    
    return revised;
}

vector<Arc> AC3Algorithm::getArcs(int var) const {
    vector<Arc> arcs;
    
    for (const auto& constraint : csp.constraints) {
        if (constraint.var1 == var) {
            arcs.push_back(Arc(constraint.var1, constraint.var2));
        } else if (constraint.var2 == var) {
            arcs.push_back(Arc(constraint.var2, constraint.var1));
        }
    }
    
    return arcs;
}

void AC3Algorithm::setDomains(const std::vector<std::vector<int>>& new_domains) {
    domains = new_domains;
}

bool AC3Algorithm::isConsistent(int var1, int val1, int var2, int val2) const {
    for (const auto& constraint : csp.constraints) {
        if ((constraint.var1 == var1 && constraint.var2 == var2) ||
            (constraint.var1 == var2 && constraint.var2 == var1)) {
            
            // Check if the pair (val1, val2) is in the allowed pairs
            bool found = false;
            for (const auto& pair : constraint.allowed_pairs) {
                if ((pair.first == val1 && pair.second == val2) ||
                    (pair.first == val2 && pair.second == val1)) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
    }
    return true;
}

bool AC3Algorithm::apply(bool verbose) {
    int iteration = 0;
    if (verbose) {
        cout << "   Initial worklist size: " << worklist.size() << endl;
    }
    
    while (!worklist.empty()) {
        iteration++;
        Arc arc = worklist.front();
        worklist.pop();
        
        assert(arc.var1 >= 0 && arc.var1 < csp.num_variables && "apply(): arc.var1 is out of bounds");
        assert(arc.var2 >= 0 && arc.var2 < csp.num_variables && "apply(): arc.var2 is out of bounds");

        if (verbose) {
            cout << "   Iteration " << iteration << ": Processing arc (" << arc.var1 << " -> " << arc.var2 << ")" << endl;
            cout << "     Domain " << arc.var1 << " before: [";
            for (size_t i = 0; i < domains[arc.var1].size(); i++) {
                cout << domains[arc.var1][i];
                if (i < domains[arc.var1].size() - 1) cout << ", ";
            }
            cout << "]" << endl;
        }
        
        if (revise(arc.var1, arc.var2)) {
            if (verbose) {
                cout << "     Domain " << arc.var1 << " after:  [";
                for (size_t i = 0; i < domains[arc.var1].size(); i++) {
                    cout << domains[arc.var1][i];
                    if (i < domains[arc.var1].size() - 1) cout << ", ";
                }
                cout << "] (REVISED)" << endl;
            }
            
            if (domains[arc.var1].empty()) {
                if (verbose) {
                    cout << "     Domain " << arc.var1 << " is empty - instance inconsistent!" << endl;
                }
                return false; // Instance inconsistent
            }
            
            // Add arcs from neighbors of var1 (except var2)
            vector<Arc> neighbors = getArcs(arc.var1);
            int added_arcs = 0;
            for (const Arc& neighbor : neighbors) {
                if (neighbor.var2 != arc.var2) {
                    worklist.push(neighbor);
                    added_arcs++;
                }
            }
            if (verbose) {
                cout << "     Added " << added_arcs << " new arcs to worklist" << endl;
            }
        } else {
            if (verbose) {
                cout << "     No revision needed for arc (" << arc.var1 << " -> " << arc.var2 << ")" << endl;
            }
        }
        
        if (verbose) {
            cout << "     Worklist size: " << worklist.size() << endl;
        }
    }
    
    if (verbose) {
        cout << "   AC-3 completed after " << iteration << " iterations" << endl;
        cout << "   Total revisions: " << revisions_count << endl;
    }
    return true; // Instance consistent
}

vector<vector<int>> AC3Algorithm::getDomains() const {
    return domains;
}

int AC3Algorithm::getRevisionsCount() const {
    return revisions_count;
}

bool AC3Algorithm::hasEmptyDomain() const {
    for (const auto& domain : domains) {
        if (domain.empty()) {
            return true;
        }
    }
    return false;
}

void AC3Algorithm::printStats() const {
    cout << "   AC-3 Statistics:" << endl;
    cout << "     Revisions performed: " << revisions_count << endl;
    cout << "     Final domains:" << endl;
    for (size_t i = 0; i < domains.size(); i++) {
        cout << "       " << i << ": [" << domains[i].size() << " values]";
        if (domains[i].size() <= 10) {
            cout << " = [";
            for (size_t j = 0; j < domains[i].size(); j++) {
                cout << domains[i][j];
                if (j < domains[i].size() - 1) cout << ", ";
            }
            cout << "]";
        }
        cout << endl;
    }
}