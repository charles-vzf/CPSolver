# CPSolver - Constraint Satisfaction Problem Solver

A Constraint Satisfaction Problem (CSP) solver implemented in C++ with support for the DIMACS format. This solver uses advanced constraint propagation algorithms and heuristic strategies to efficiently solve CSP instances.

## Project Architecture

```
Solver/
├── main.cpp                    # Main entry point with argument handling
├── Makefile                    # Compilation configuration
├── README.md                   # Complete documentation
├── solve_all.sh                # Batch solving script
├── trace_solve_all.txt         # Solving logs
└── src/                        # Modular source code
    ├── core/                   # Core structures and parameters
    │   └── params.h            # Solver configuration
    ├── parser/                 # CSP file parsing
    │   ├── parser.h            # DIMACS parsing interface
    │   └── parser.cpp          # Parser implementation
    ├── solver/                 # Main solver logic
    │   ├── solver.h            # Main CSPSolver class
    │   └── solver.cpp          # Backtracking algorithm
    ├── algorithms/             # Consistency algorithms
    │   ├── ac3.h               # AC-3 interface
    │   └── ac3.cpp             # AC-3 implementation
    ├── strategies/             # Selection strategies
    │   ├── strategies.h        # Selection heuristics
    │   └── strategies.cpp      # MRV, Degree, LCV, etc.
    └── io/                     # Input/Output
        ├── solution_writer.h   # Solution writing
        ├── solution_writer.cpp
        └── logo.h              # User interface
```

## Implemented Algorithms

### AC-3 Algorithm (Arc Consistency)

The AC-3 algorithm, implemented in `src/algorithms/ac3.cpp`, is a fundamental tool for constraint propagation. Its primary goal is to establish arc consistency, which significantly prunes the search space.

An arc `(X, Y)` is considered arc-consistent if for every value `x` in the domain of `X`, there exists a corresponding value `y` in the domain of `Y` such that the constraint between `X` and `Y` is satisfied. The AC-3 algorithm systematically enforces arc consistency across all constraints in the problem.

- **Constraint Propagation**: It iteratively removes values from variable domains that cannot be part of any valid solution.
- **Inconsistency Detection**: If any variable's domain becomes empty during propagation, it indicates that the CSP instance has no solution, allowing for early termination.
- **Search Space Reduction**: By reducing domains, AC-3 decreases the number of nodes the backtracking algorithm needs to explore.
- **Flexible Application**: It can be applied once at the beginning of the search (initial pruning) and/or at each node of the search tree (maintaining arc consistency).

**Technical Features:**
- Uses a worklist (queue) of arcs to efficiently manage consistency checks.
- Tracks the number of revisions for statistical analysis.
- Supports a verbose mode with detailed traces for debugging.
- Detects empty domains to immediately signal inconsistency.

### Selection Strategies (`src/strategies/`)

The `SelectionStrategies` class implements several heuristics to guide the backtracking search, which is critical for performance.

#### Variable Selection (Branching)

- **MRV (Minimum Remaining Values)**: This "fail-first" heuristic selects the variable with the smallest domain. The intuition is to choose the most constrained variable, thereby encountering dead-ends more quickly and pruning the search tree earlier.
- **Degree Heuristic**: Selects the variable involved in the largest number of constraints with other unassigned variables. This is often used as a tie-breaker for MRV and helps in reducing the branching factor of the search tree.
- **Random**: Random selection for performance baseline testing.

#### Value Ordering

- **LCV (Least Constraining Value)**: This "succeed-first" heuristic prefers values that rule out the fewest choices for neighboring variables in the constraint graph. It attempts to leave maximum flexibility for subsequent assignments, increasing the likelihood of finding a solution without backtracking.
- **Lexicographic**: Natural ordering of values.
- **Random**: Random value ordering.

### Backtracking Algorithm

The main solver (`src/solver/solver.cpp`) implements an intelligent backtracking search:

- **Depth-First Search**: Explores possible assignments in a depth-first manner.
- **Forward Checking**: A lighter form of propagation. When a value is assigned to a variable, it checks all constraints involving that variable and removes any inconsistent values from the domains of neighboring unassigned variables.
- **Time Management**: Configurable time limit for the search.
- **Detailed Statistics**: Tracks explored nodes, backtracks, and execution time.
- **Multi-solution Support**: Can find all solutions or stop at the first one.

## Compilation

```bash
make                    # Standard compilation
make clean              # Clean up build files
make debug              # Compile in debug mode
make release            # Compile with optimizations
```

## Configuration Parameters

The solver uses the `SolverParams` struct defined in `src/core/params.h` to manage all settings:

### Time and Solution Limits
- `max_time` (default: 300): Maximum time in seconds.
- `first_solution_only` (default: false): Stop after finding the first solution.

### Search Strategies
- `var_strategy` (default: "mrv"): Variable selection strategy.
  - `"mrv"`: Minimum Remaining Values
  - `"degree"`: Degree Heuristic
  - `"random"`: Random selection
- `val_strategy` (default: "lcv"): Value ordering strategy.
  - `"lcv"`: Least Constraining Value
  - `"random"`: Random ordering
  - `"lexicographic"`: Lexicographic ordering

### Constraint Propagation
- `use_ac3` (default: true): Enable the AC-3 algorithm.
- `use_forward_checking` (default: true): Enable forward checking.
- `ac3_at_each_node` (default: true): Apply AC-3 at each node in the backtracking search.

### Output Control
- `verbose` (default: false): Verbose mode with detailed traces.
- `max_depth_trace` (default: 5): Maximum depth for detailed traces.
- `max_depth_ac3_trace` (default: 3): Maximum depth for AC-3 traces.
- `show_global_stats_only` (default: false): Display only global statistics.
- `output_path` (default: ""): Custom output path for solutions.

## Usage

```bash
./CPSolver <file.csp> [options]

Available options:
  -t <time>      Maximum time in seconds (default: 300)
  -f             Stop at the first solution found
  -v <strategy>  Variable selection strategy: mrv, degree, random
  -w <strategy>  Value selection strategy: lcv, random, lexicographic
  -a             Disable AC-3 completely
  -c             Disable forward checking
  -n             Disable AC-3 at each node (keep initial AC-3)
  -o <path>      Custom output path
  -V             Verbose mode (detailed traces)
  -h             Display full help
```

## Usage Examples

```bash
# Standard solve with all optimizations
./CPSolver ../instances/instances/equality_example.csp

# First solution only (faster)
./CPSolver ../instances/instances/equality_example.csp -f

# Custom strategies
./CPSolver ../instances/instances/equality_example.csp -v degree -w random

# Disable optimizations for comparison
./CPSolver ../instances/instances/equality_example.csp -a -c

# Verbose mode with detailed traces
./CPSolver ../instances/instances/equality_example.csp -V

# Custom time limit
./CPSolver ../instances/instances/equality_example.csp -t 60

# Custom output
./CPSolver ../instances/instances/equality_example.csp -o my_solution.sol

# Full configuration for performance testing
./CPSolver ../instances/instances/equality_example.csp -t 120 -v mrv -w lcv -V -o perf_test.sol
```

## File Formats

### CSP Instance (DIMACS format)
```
# Comments
4                           # Number of variables

# Domains (variable_id min_value max_value)
0 1 3
1 1 3
2 1 3
3 1 3

4                           # Number of constraints

# Constraints (var1 var2 (val1,val2) (val3,val4) ...)
2 3 (1,1) (2,2) (3,3)
0 1 (1,1) (2,2) (3,3)
1 3 (1,1) (2,2) (3,3)
0 3 (1,1) (2,2) (3,3)
```

### Solution (.sol)
```
# Solution for 4 variables
# Variables: 4, Domain size: 3
# Number of solutions found: 3

# Solution 1
0=1 1=1 2=1 3=1

# Solution 2
0=2 1=2 2=2 3=2

# Solution 3
0=3 1=3 2=3 3=3
```

## Detailed Architecture

### Main Components

#### 1. Parser (`src/parser/`)
- **parser.h/cpp**: Parses CSP files in DIMACS format.
- Parses variables, domains, and constraints.
- Validates syntax and detects errors.
- Uses a `CSPInstance` data structure to represent the problem.

#### 2. Main Solver (`src/solver/`)
- **solver.h/cpp**: `CSPSolver` class with the backtracking algorithm.
- Manages variable assignments.
- Integrates with AC-3 and forward checking.
- Collects performance statistics (nodes, backtracks, time).

#### 3. Consistency Algorithms (`src/algorithms/`)
- **ac3.h/cpp**: `AC3Algorithm` class for constraint propagation.
- Implements the AC-3 algorithm with a worklist.
- Manages domains and detects inconsistencies.
- Tracks revision statistics.

#### 4. Selection Strategies (`src/strategies/`)
- **strategies.h/cpp**: `SelectionStrategies` class for heuristics.
- Implements MRV, Degree, LCV, and random strategies.
- Calculates conflicts and interactions between variables.
- Generates random numbers for testing.

#### 5. Input/Output (`src/io/`)
- **solution_writer.h/cpp**: Writes solutions in text format.
- **logo.h**: User interface with a logo and formatted output.
- Manages output files and directory creation.

#### 6. Configuration (`src/core/`)
- **params.h**: `SolverParams` struct for all parameters.
- Provides default values and validates options.
- Centralized configuration for solver behavior.

## Performance

The solver is optimized for:
- Finding all possible solutions.
- Minimizing the search space with AC-3 and forward checking.
- Providing detailed statistics.