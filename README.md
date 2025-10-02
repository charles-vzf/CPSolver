```
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║    ██████╗██████╗      ███████╗ ██████╗ ██╗    ██╗ ╔██ ███████╗██████╗       ║
║   ██╔════╝██╔══██╗     ██╔════╝██╔═══██╗██║    ██║ ║██ ██╔════╝██╔══██╗      ║
║   ██║     ██████╔╝ ██  ███████╗██║   ██║██║    █║   ║█ █████╗  ██████╔╝      ║
║   ██║     ██╔═══╝      ╚════██║██║   ██║██║    ║█   █║ ██╔══╝  ██╔══██╗      ║
║   ╚██████╗██║          ███████║╚██████╔╝╚██████╚█████╝ ███████╗██║  ██║      ║
║    ╚═════╝╚═╝          ╚══════╝ ╚═════╝  ╚════╝ ╚═══╝  ╚══════╝╚═╝  ╚═╝      ║
║                                                                              ║
║              Constraint Satisfaction Problem Solver                          ║
║                    By Erwann Esteve & Charles Vielzeuf                       ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
```
A constraint programming solver developed as part of a Parisian Master of Operations Research Constraint Programming class ([https://www.utc.fr/~dsavoure/teaching/ppc/index.html](https://www.utc.fr/~dsavoure/teaching/ppc/index.html)).

## About Constraint Satisfaction Problems (CSPs)

A Constraint Satisfaction Problem (CSP) is defined by:
- **Variables**: A finite set of variables, each with a domain of possible values
- **Constraints**: A finite set of constraints that restrict the combinations of values that variables can take

In this solver, we focus on **binary CSPs** with **integer variables**, where:
- Each variable has a finite domain of integer values
- Constraints are binary (involving exactly two variables)
- Constraints are defined in **extension** (explicitly listing allowed value pairs) rather than in **intension** (using mathematical expressions)

### Extension vs Intension

- **Extension**: Constraints are defined by explicitly listing all allowed combinations of values
  - Example: `(x1=1, x2=2), (x1=2, x2=1), (x1=3, x2=3)`
- **Intension**: Constraints are defined using mathematical expressions
  - Example: `x1 + x2 = 3` or `x1 ≠ x2`

## DIMACS CSP Format

This solver uses the **DIMACS CSP format** (Discrete Mathematics and Theoretical Computer Science), a standard format for representing Constraint Satisfaction Problems.

### Input File Format (.csp)

The solver expects input files with the following DIMACS CSP format:

```
# CSP with inequality constraints
# Generated CSP instance
# Variables: 4, Domain size: 3
# Constraints: 4

4

# Variable domains (variable_id min_value max_value)
0 1 3
1 1 3
2 1 3
3 1 3

4

# Constraints (var1 var2 (value1,value2) (value3,value4) ...)
2 3 (1,2) (1,3) (2,1) (2,3) (3,1) (3,2)
0 1 (1,2) (1,3) (2,1) (2,3) (3,1) (3,2)
1 3 (1,2) (1,3) (2,1) (2,3) (3,1) (3,2)
0 3 (1,2) (1,3) (2,1) (2,3) (3,1) (3,2)
```

### Format Structure

1. **Header Comments**: Lines starting with `#` or `b` containing metadata
2. **Number of Variables**: Single integer indicating total variables (numbered 0 to n-1)
3. **Variable Domains**: For each variable: `variable_id min_value max_value`
4. **Number of Constraints**: Single integer indicating total binary constraints
5. **Constraints**: For each constraint: `var1 var2 (val1,val2) (val3,val4) ...`

### Constraint Types

- **Inequality**: `(1,2) (1,3) (2,1) (2,3) (3,1) (3,2)` means var1 ≠ var2
- **Equality**: `(1,1) (2,2) (3,3)` means var1 = var2
- **Sum Target**: `(1,3) (2,2) (3,1)` means var1 + var2 = 4

## Solution File Format (.sol)

Solution files follow the DIMACS CSP solution format and contain one or more valid assignments with resolution parameters:

```
# ┌─────────────────────────────────────────────────────────────────────────────┐
# │                              SOLVER STATISTICS                              │
# └─────────────────────────────────────────────────────────────────────────────┘
# Generated on: 2025-10-02 12:56:40
# Variables: 4
# Constraints: 4
# Domain size: 3
# Solutions found: 3
# Resolution status: All solutions found
# Nodes explored: 48
# Solving time: 0ms
# Variable strategy: mrv
# Value strategy: lcv
# AC-3: Enabled
# Forward checking: Enabled
#
# Solution 1
0=1 1=1 2=1 3=1

# Solution 2
0=2 1=2 2=2 3=2

# Solution 3
0=3 1=3 2=3 3=3
```

### Solution Format Structure

1. **Header Comments**: Instance metadata and solution count
2. **Resolution Parameters**: Solver performance metrics
   - `Number of nodes`: Nodes explored in the search tree
   - `Relative gap`: Optimality gap (0.0 for optimal solutions)
3. **Solutions**: Each solution as `variable_id=value` assignments

## Usage

### Using the Gurobi Solver (Julia)

1. Place your CSP instance files in the `instances/instances/` folder
2. Use the `solve(instance_name)` function in the `gurobi_guard.ipynb` notebook:

```julia
# Solve a single instance
solution = solve("inequality_example.csp")

# List available instances
instances = list_available_instances()

# Batch solve multiple instances
results = batch_solve(instances)
```

### Using the Instance Generator

Use the `generator.ipynb` notebook to create custom DIMACS CSP instances:

```julia
# Generate a new instance
generate_csp_instance("my_problem", 5, 3, 8, "inequality")
```

## Instances

The `instances/` folder contains DIMACS CSP format instances and generation tools.

### Instance Structure

```
instances/
├── description.md          # DIMACS CSP format documentation
├── generator.ipynb         # Jupyter notebook for generating instances
└── instances/              # Instance files (.csp)
    ├── inequality_example.csp
    ├── equality_example.csp
    ├── sum_target_example.csp
    └── [other instances...]
```

### Example Instances

- `inequality_example.csp`: 4-variable problem with inequality constraints
- `equality_example.csp`: 4-variable problem with equality constraints  
- `sum_target_example.csp`: 4-variable problem with sum target constraints
- `small_*.csp`, `medium_*.csp`, `large_*.csp`: Various sized instances

### Instance Generator

The `generator.ipynb` notebook provides functions to generate DIMACS CSP instances with different constraint types and parameters.

## Solutions

The `solutions/` folder contains solution files and validation tools.

### Solution Structure

```
solutions/
├── description.md          # Solution format documentation
├── validate_sol.ipynb      # Solution validation notebook
└── solutions/              # Solution files (.sol)
    ├── inequality_example.sol
    ├── equality_example.sol
    └── [other solutions...]
```

### Solution Validation

Use the `validate_sol.ipynb` notebook to validate solution files against their corresponding instances.

## Project Structure

```
CPSolver/
├── README.md
├── gurobi_guard.ipynb      # Main Gurobi solver implementation
├── instances/
│   ├── description.md      # DIMACS CSP format documentation
│   ├── generator.ipynb     # Instance generator notebook
│   └── instances/          # Instance files (.csp)
├── solutions/
│   ├── description.md      # Solution format documentation
│   ├── validate_sol.ipynb  # Solution validation notebook
│   └── solutions/          # Solution files (.sol)
└── [other solver files...]
```

## Development

This project was developed as part of the Constraint Programming course at UTC, focusing on:
- Backtracking algorithms
- Arc consistency techniques
- Look-ahead and look-back methods
- Binary constraint satisfaction problems

## License

This project is developed for educational purposes as part of the UTC Master's program in Operations Research.
