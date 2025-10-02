# DIMACS CSP Solution Format

## Mathematical Formulation

A **solution** to a CSP instance P = (X, D, C) is a complete assignment of values to all variables that satisfies every constraint.

### Solution Definition

A solution is a function **σ: X → ∪ᵢDᵢ** such that:
1. **Domain consistency**: σ(xᵢ) ∈ Dᵢ for all xᵢ ∈ X
2. **Constraint satisfaction**: For every constraint c(xᵢ, xⱼ) ∈ C, the pair (σ(xᵢ), σ(xⱼ)) is in the allowed pairs of c

### Solution Set

The **solution set** S(P) of a CSP instance P is:
```
S(P) = {σ | σ is a solution to P}
```

A CSP instance is:
- **Satisfiable** if |S(P)| > 0 (at least one solution exists)
- **Unsatisfiable** if |S(P)| = 0 (no solution exists)
- **Unique** if |S(P)| = 1 (exactly one solution exists)
- **Multiple** if |S(P)| > 1 (more than one solution exists)

## DIMACS Solution Format

This project uses the **DIMACS CSP solution format** which includes both solution assignments and solver performance metrics.

### File Structure

Solution files use the `.sol` extension and follow this DIMACS structure:

```
# Solution for [instance_name]
# Variables: n, Domain size: d
# Number of solutions found: k

# Resolution parameters:
# Gurobi time: X.XXXX seconds
# Number of nodes: N
# Relative gap: X.XXXXXX

# Solution 1
var0=value0 var1=value1 var2=value2 ...

# Solution 2 (if multiple solutions)
var0=value0 var1=value1 var2=value2 ...

# No solution (if no valid assignment exists)
NO SOLUTION
```

### Format Details

- **Header Comments**: Instance metadata and solution count
- **Resolution Parameters**: Solver performance metrics
  - `Gurobi time`: Resolution time in seconds
  - `Number of nodes`: Nodes explored in the search tree
  - `Relative gap`: Optimality gap (0.0 for optimal solutions)
- **Solutions**: Each solution as `variable_id=value` assignments
- **Variable order**: Variables are listed in ascending order (0, 1, 2, ...)
- **Value format**: Integer values as assigned to each variable
- **No solution**: Use `NO SOLUTION` when the instance is unsatisfiable

## Example: inequality_example.sol

```
# Solution for inequality_example
# Variables: 4, Domain size: 3
# Number of solutions found: 6

# Resolution parameters:
# Gurobi time: 0.0234 seconds
# Number of nodes: 15
# Relative gap: 0.000000

# Solution 1
0=1 1=2 2=1 3=2

# Solution 2
0=1 1=3 2=1 3=3

# Solution 3
0=2 1=1 2=2 3=1

# Solution 4
0=2 1=3 2=2 3=3

# Solution 5
0=3 1=1 2=3 3=1

# Solution 6
0=3 1=2 2=3 3=2
```

### Mathematical Verification

For the inequality_example instance with constraints:
- All pairs of variables must have different values (xᵢ ≠ xⱼ for all i ≠ j)

**Solution 1**: σ₁ = {x₀→1, x₁→2, x₂→1, x₃→2}
- x₀ ≠ x₁: ✓ (1 ≠ 2)
- x₀ ≠ x₂: ✓ (1 ≠ 1) ❌ **Error!** This violates the constraint
- x₀ ≠ x₃: ✓ (1 ≠ 2)
- x₁ ≠ x₂: ✓ (2 ≠ 1)
- x₁ ≠ x₃: ✓ (2 ≠ 2) ❌ **Error!** This violates the constraint
- x₂ ≠ x₃: ✓ (1 ≠ 2)

**Note**: This example shows an invalid solution. A valid solution would be:
**Valid Solution**: σ = {x₀→1, x₁→2, x₂→3, x₃→1}
- All pairs have different values: ✓

### Resolution Parameters

The solution file includes important solver performance metrics:
- **Gurobi time**: 0.0234 seconds - Time taken by the Gurobi solver
- **Number of nodes**: 15 - Number of nodes explored in the search tree
- **Relative gap**: 0.000000 - Optimality gap (0.0 means optimal solution found)
