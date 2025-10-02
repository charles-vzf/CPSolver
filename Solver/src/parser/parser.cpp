#include "parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cmath>

using namespace std;

// Function to remove leading and trailing spaces
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Function to split a string by delimiter
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Check if a line is a comment
bool isComment(const string& line) {
    string trimmed = trim(line);
    return trimmed.empty() || trimmed[0] == '#';
}

// Check if a line is empty
bool isEmpty(const string& line) {
    return trim(line).empty();
}

// Parse domain line: "var_id min_value max_value"
pair<int, int> parseDomainLine(const string& line) {
    vector<string> tokens = split(line, ' ');
    if (tokens.size() != 3) {
        throw runtime_error("Invalid domain line format: " + line);
    }
    
    // var_id is not used but we validate it exists
    stoi(tokens[0]); // Validate var_id is a number
    int min_val = stoi(tokens[1]);
    int max_val = stoi(tokens[2]);
    
    return make_pair(min_val, max_val);
}

// Parse constraint line: "var1 var2 (val1,val2) (val3,val4) ..."
vector<pair<int, int>> parseConstraintLine(const string& line) {
    vector<string> tokens = split(line, ' ');
    if (tokens.size() < 2) {
        throw runtime_error("Invalid constraint line format: " + line);
    }
    
    vector<pair<int, int>> pairs;
    
    // Skip the first two tokens (var1 var2) and parse the pairs
    for (size_t i = 2; i < tokens.size(); i++) {
        string pair_str = tokens[i];
        
        // Remove parentheses
        if (pair_str[0] == '(' && pair_str[pair_str.length()-1] == ')') {
            pair_str = pair_str.substr(1, pair_str.length()-2);
        }
        
        // Split by comma
        size_t comma_pos = pair_str.find(',');
        if (comma_pos == string::npos) {
            throw runtime_error("Invalid pair format: " + pair_str);
        }
        
        int val1 = stoi(trim(pair_str.substr(0, comma_pos)));
        int val2 = stoi(trim(pair_str.substr(comma_pos + 1)));
        
        pairs.push_back(make_pair(val1, val2));
    }
    
    return pairs;
}

// CSPInstance class methods
bool CSPInstance::hasVariable(int var) const {
    return var >= 0 && var < num_variables;
}

vector<int> CSPInstance::getNeighbors(int var) const {
    set<int> neighbors;
    for (const Constraint& c : constraints) {
        if (c.var1 == var) {
            neighbors.insert(c.var2);
        } else if (c.var2 == var) {
            neighbors.insert(c.var1);
        }
    }
    return vector<int>(neighbors.begin(), neighbors.end());
}

vector<Constraint> CSPInstance::getConstraints(int var) const {
    vector<Constraint> result;
    for (const Constraint& c : constraints) {
        if (c.var1 == var || c.var2 == var) {
            result.push_back(c);
        }
    }
    return result;
}

bool CSPInstance::isConsistent(int var1, int val1, int var2, int val2) const {
    for (const Constraint& c : constraints) {
        if (c.var1 == var1 && c.var2 == var2) {
            // Check if the pair (val1, val2) is in the allowed pairs
            bool found = false;
            for (const auto& pair : c.allowed_pairs) {
                if (pair.first == val1 && pair.second == val2) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        } else if (c.var1 == var2 && c.var2 == var1) {
            // Check if the pair (val2, val1) is in the allowed pairs
            bool found = false;
            for (const auto& pair : c.allowed_pairs) {
                if (pair.first == val2 && pair.second == val1) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
    }
    return true;
}

vector<int> CSPInstance::getDomain(int var) const {
    vector<int> domain;
    if (var >= 0 && var < num_variables) {
        int min_val = domains[var].first;
        int max_val = domains[var].second;
        for (int val = min_val; val <= max_val; val++) {
            domain.push_back(val);
        }
    }
    return domain;
}

// Main parsing function for DIMACS format
CSPInstance parseCSPFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filename);
    }
    
    CSPInstance csp;
    string line;
    int line_number = 0;
    
    // Read number of variables
    while (getline(file, line)) {
        line_number++;
        line = trim(line);
        
        if (isComment(line) || isEmpty(line)) {
            continue;
        }
        
        csp.num_variables = stoi(line);
        break;
    }
    
    if (csp.num_variables <= 0) {
        throw runtime_error("Invalid number of variables: " + to_string(csp.num_variables));
    }
    
    // Read domain definitions
    csp.domains.resize(csp.num_variables);
    int domains_read = 0;
    
    while (getline(file, line) && domains_read < csp.num_variables) {
        line_number++;
        line = trim(line);
        
        if (isComment(line) || isEmpty(line)) {
            continue;
        }
        
        try {
            auto domain = parseDomainLine(line);
            csp.domains[domains_read] = domain;
            domains_read++;
        } catch (const exception& e) {
            throw runtime_error("Error parsing domain at line " + to_string(line_number) + ": " + e.what());
        }
    }
    
    // Continue reading until we find the number of constraints
    while (getline(file, line)) {
        line_number++;
        string original_line = line;
        line = trim(line);
        
        // cout << "DEBUG: Reading constraint count line " << line_number << ": original='" << original_line << "', trimmed='" << line << "'" << endl;
        
        if (isComment(line) || isEmpty(line)) {
            // cout << "DEBUG: Skipping line (comment or empty)" << endl;
            continue;
        }
        
        // This should be the number of constraints
        // cout << "DEBUG: Found constraint count line: '" << line << "'" << endl;
        break;
    }
    
    if (domains_read != csp.num_variables) {
        throw runtime_error("Expected " + to_string(csp.num_variables) + " domain definitions, got " + to_string(domains_read));
    }
    
    // Read number of constraints (we already have the line from the previous loop)
    int num_constraints = 0;
    try {
        num_constraints = stoi(line);
        // cout << "DEBUG: Found " << num_constraints << " constraints" << endl;
    } catch (const exception& e) {
        throw runtime_error("Error parsing number of constraints at line " + to_string(line_number) + ": " + e.what());
    }
    
    // Read constraints
    int constraints_read = 0;
    
    while (getline(file, line) && constraints_read < num_constraints) {
        line_number++;
        line = trim(line);
        
        // cout << "DEBUG: Reading line " << line_number << ": '" << line << "'" << endl;
        
        if (isComment(line) || isEmpty(line)) {
            continue;
        }
        
        
        try {
            vector<string> tokens = split(line, ' ');
            if (tokens.size() < 2) {
                throw runtime_error("Invalid constraint line: " + line);
            }
            
            int var1 = stoi(tokens[0]);
            int var2 = stoi(tokens[1]);
            
            if (var1 < 0 || var1 >= csp.num_variables || var2 < 0 || var2 >= csp.num_variables) {
                throw runtime_error("Invalid variable IDs in constraint: " + line);
            }
            
            Constraint constraint(var1, var2);
            
            // Parse allowed pairs
            for (size_t i = 2; i < tokens.size(); i++) {
                string pair_str = tokens[i];
                
                // Remove parentheses
                if (pair_str[0] == '(' && pair_str[pair_str.length()-1] == ')') {
                    pair_str = pair_str.substr(1, pair_str.length()-2);
                }
                
                // Split by comma
                size_t comma_pos = pair_str.find(',');
                if (comma_pos == string::npos) {
                    throw runtime_error("Invalid pair format: " + pair_str);
                }
                
                int val1 = stoi(trim(pair_str.substr(0, comma_pos)));
                int val2 = stoi(trim(pair_str.substr(comma_pos + 1)));
                
                constraint.allowed_pairs.push_back(make_pair(val1, val2));
            }
            
            csp.constraints.push_back(constraint);
            constraints_read++;
            
        } catch (const exception& e) {
            throw runtime_error("Error parsing constraint at line " + to_string(line_number) + ": " + e.what());
        }
    }
    
    file.close();
    
    return csp;
}