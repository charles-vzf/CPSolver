#ifndef SOLUTION_WRITER_H
#define SOLUTION_WRITER_H

#include <string>
#include <vector>
#include <map>
#include "../parser/parser.h"
#include "../core/params.h"

// Function to write solutions to a file
void writeSolutions(const std::string& filename, 
                    const std::vector<std::map<int, int>>& solutions,
                    const CSPInstance& csp,
                    const SolverParams& params,
                    long duration_ms,
                    int nodes_explored,
                    const std::string& resolution_status);

#endif // SOLUTION_WRITER_H
