# CPSolver - Constraint Satisfaction Problem Solver

Un solveur de problèmes de satisfaction de contraintes (CSP) implémenté en C++ avec support du format DIMACS.

## Structure du projet

```
Solver/
├── main.cpp                    # Point d'entrée principal
├── Makefile                    # Configuration de compilation
├── README.md                   # Documentation
└── src/                        # Code source organisé
    ├── parser/                 # Analyse des fichiers CSP
    │   ├── parser.h
    │   └── parser.cpp
    ├── solver/                 # Logique principale du solveur
    │   ├── solver.h
    │   └── solver.cpp
    ├── algorithms/             # Algorithmes de consistance
    │   ├── ac3.h
    │   └── ac3.cpp
    ├── strategies/             # Stratégies de sélection
    │   ├── strategies.h
    │   └── strategies.cpp
    └── io/                     # Entrée/Sortie
        ├── solution_writer.h
        └── solution_writer.cpp
```

## Fonctionnalités

- **Format DIMACS** : Support complet du format DIMACS pour les instances CSP
- **Algorithmes de consistance** : AC-3 pour la propagation de contraintes
- **Stratégies de sélection** :
  - Variables : MRV (Minimum Remaining Values), Degree, Random
  - Valeurs : LCV (Least Constraining Value), Random, Lexicographic
- **Forward Checking** : Vérification anticipée des contraintes
- **Recherche complète** : Trouve toutes les solutions possibles
- **Gestion du temps** : Limite de temps configurable
- **Sortie détaillée** : Traces de résolution et statistiques

## Compilation

```bash
make                    # Compilation standard
make clean             # Nettoyage
make debug             # Compilation en mode debug
make release           # Compilation optimisée
```

## Utilisation

```bash
./CPSolver <fichier.csp> [options]

Options:
  -t <time>      Temps maximum en secondes (défaut: 300)
  -f             Arrêter à la première solution
  -v <strategy>  Stratégie de sélection des variables: mrv, degree, random
  -w <strategy>  Stratégie de sélection des valeurs: lcv, random, lexicographic
  -a             Désactiver AC-3
  -c             Désactiver forward checking
  -q             Mode silencieux
  -h             Afficher l'aide
```

## Exemples

```bash
# Résolution standard
./CPSolver ../instances/instances/equality_example.csp

# Première solution seulement
./CPSolver ../instances/instances/equality_example.csp -f

# Stratégies personnalisées
./CPSolver ../instances/instances/equality_example.csp -v degree -w random

# Sans AC-3 ni forward checking
./CPSolver ../instances/instances/equality_example.csp -a -c
```

## Format des fichiers

### Instance CSP (format DIMACS)
```
# Commentaires
4                           # Nombre de variables

# Domaines (variable_id min_value max_value)
0 1 3
1 1 3
2 1 3
3 1 3

4                           # Nombre de contraintes

# Contraintes (var1 var2 (val1,val2) (val3,val4) ...)
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

## Architecture

- **Parser** : Analyse les fichiers CSP au format DIMACS
- **Solver** : Implémente l'algorithme de backtracking avec forward checking
- **AC-3** : Algorithme de consistance d'arc pour la propagation de contraintes
- **Strategies** : Stratégies de sélection des variables et valeurs
- **IO** : Gestion de l'écriture des solutions

## Performance

Le solveur est optimisé pour :
- Trouver toutes les solutions possibles
- Minimiser l'espace de recherche avec AC-3 et forward checking
- Gérer efficacement les grandes instances
- Fournir des statistiques détaillées