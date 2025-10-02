# DIMACS CSP Instance Format

## Mathematical Formulation

A Constraint Satisfaction Problem (CSP) is formally defined as a triple **P = (X, D, C)** where:

- **X = {x₁, x₂, ..., xₙ}** is a finite set of **n variables**
- **D = {D₁, D₂, ..., Dₙ}** is a set of **domains**, where each Dᵢ is the finite set of possible values for variable xᵢ
- **C = {c₁, c₂, ..., cₘ}** is a finite set of **constraints**, where each constraint cⱼ restricts the values that a subset of variables can simultaneously take

### Binary CSPs

In this solver, we focus on **binary CSPs** where:
- Each constraint involves exactly **two variables**
- All variables have **integer domains** of the form Dᵢ = {minᵢ, minᵢ+1, ..., maxᵢ}
- Constraints are defined in **extension** (explicitly listing allowed value pairs)

### Constraint Representation

A binary constraint c(xᵢ, xⱼ) between variables xᵢ and xⱼ is represented as:
```
c(xᵢ, xⱼ) = {(vᵢ, vⱼ) | (vᵢ, vⱼ) ∈ Dᵢ × Dⱼ and the constraint is satisfied}
```

## DIMACS CSP Format

This project uses the **DIMACS CSP format** (Discrete Mathematics and Theoretical Computer Science), a standard format for representing Constraint Satisfaction Problems.

### File Structure

CSP instance files use the `.csp` extension and follow this DIMACS structure:

```
# CSP with constraint_type constraints
# Generated CSP instance
# Variables: n, Domain size: d
# Constraints: m

n

# Variable domains (variable_id min_value max_value)
0 min₀ max₀
1 min₁ max₁
...
n-1 minₙ₋₁ maxₙ₋₁

m

# Constraints (var1 var2 (value1,value2) (value3,value4) ...)
var1 var2 (val1,val2) (val3,val4) ...
var1 var2 (val1,val2) (val3,val4) ...
...
```

### Format Details

- **Header Comments**: Lines starting with `#` or `b` containing metadata about the instance
- **Variables**: Numbered from 0 to n-1
- **Domains**: Each variable has a domain [min_value, max_value] of consecutive integers
- **Constraints**: Each constraint lists all allowed pairs of values for the two variables
- **Comments**: Lines starting with `#` or `b` are ignored and can contain descriptions

## Constraint Types

### Inequality Constraints
Variables must have different values:
```
# Constraint: var1 ≠ var2
var1 var2 (1,2) (1,3) (2,1) (2,3) (3,1) (3,2)
```

### Equality Constraints  
Variables must have the same value:
```
# Constraint: var1 = var2
var1 var2 (1,1) (2,2) (3,3)
```

### Sum Target Constraints
Variables must sum to a specific target:
```
# Constraint: var1 + var2 = 4
var1 var2 (1,3) (2,2) (3,1)
```

## Example Instances

### Example 1: inequality_example.csp

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

**Mathematical Interpretation:**
- **Variables**: X = {x₀, x₁, x₂, x₃}
- **Domains**: D₀ = D₁ = D₂ = D₃ = {1, 2, 3}
- **Constraints**: All pairs of variables must have different values (xᵢ ≠ xⱼ for all i ≠ j)

### Example 2: equality_example.csp

```
# CSP with equality constraints
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
2 3 (1,1) (2,2) (3,3)
0 1 (1,1) (2,2) (3,3)
1 3 (1,1) (2,2) (3,3)
0 3 (1,1) (2,2) (3,3)
```

**Mathematical Interpretation:**
- **Variables**: X = {x₀, x₁, x₂, x₃}
- **Domains**: D₀ = D₁ = D₂ = D₃ = {1, 2, 3}
- **Constraints**: All pairs of variables must have equal values (xᵢ = xⱼ for all i ≠ j)

### Example 3: sum_target_example.csp

```
# CSP with sum_target constraints
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
2 3 (1,3) (2,2) (3,1)
0 1 (1,2) (2,1)
1 3 (1,2) (2,1)
0 3 (1,2) (2,1)
```

**Mathematical Interpretation:**
- **Variables**: X = {x₀, x₁, x₂, x₃}
- **Domains**: D₀ = D₁ = D₂ = D₃ = {1, 2, 3}
- **Constraints**: 
  - x₂ + x₃ = 4
  - x₀ + x₁ = 3
  - x₁ + x₃ = 3
  - x₀ + x₃ = 3
