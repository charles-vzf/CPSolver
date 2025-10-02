# CPSolver - Constraint Satisfaction Problem Solver

Un solveur de problèmes de satisfaction de contraintes (CSP) implémenté en C++ avec support du format DIMACS. Ce solveur utilise des algorithmes avancés de propagation de contraintes et des stratégies heuristiques pour résoudre efficacement les instances CSP.

## Architecture du projet

```
Solver/
├── main.cpp                    # Point d'entrée principal avec gestion des arguments
├── Makefile                    # Configuration de compilation
├── README.md                   # Documentation complète
├── solve_all.sh               # Script de résolution en lot
├── trace_solve_all.txt        # Logs de résolution
└── src/                        # Code source modulaire
    ├── core/                   # Structures et paramètres
    │   └── params.h           # Configuration du solveur
    ├── parser/                 # Analyse des fichiers CSP
    │   ├── parser.h           # Interface de parsing DIMACS
    │   └── parser.cpp         # Implémentation du parser
    ├── solver/                 # Logique principale du solveur
    │   ├── solver.h           # Classe CSPSolver principale
    │   └── solver.cpp         # Algorithme de backtracking
    ├── algorithms/             # Algorithmes de consistance
    │   ├── ac3.h              # Interface AC-3
    │   └── ac3.cpp            # Implémentation AC-3
    ├── strategies/             # Stratégies de sélection
    │   ├── strategies.h       # Heuristiques de sélection
    │   └── strategies.cpp     # MRV, Degree, LCV, etc.
    └── io/                     # Entrée/Sortie
        ├── solution_writer.h  # Écriture des solutions
        ├── solution_writer.cpp
        └── logo.h             # Interface utilisateur
```

## Algorithmes implémentés

### Algorithme AC-3 (Arc Consistency)
L'algorithme AC-3 est implémenté dans `src/algorithms/ac3.cpp` et permet de :
- **Propagation de contraintes** : Élimine les valeurs inconsistantes des domaines
- **Détection d'inconsistance** : Identifie rapidement les instances sans solution
- **Réduction d'espace de recherche** : Diminue le nombre de nœuds à explorer
- **Application flexible** : Peut être appliqué globalement et/ou à chaque nœud

**Caractéristiques techniques :**
- Utilise une file de travail (worklist) pour traiter les arcs
- Compte le nombre de révisions effectuées pour les statistiques
- Support du mode verbeux avec traces détaillées
- Gestion des domaines vides pour détecter l'inconsistance

### Stratégies de sélection (src/strategies/)
La classe `SelectionStrategies` implémente plusieurs heuristiques :

#### Sélection de variables
- **MRV (Minimum Remaining Values)** : Sélectionne la variable avec le plus petit domaine
- **Degree Heuristic** : Sélectionne la variable impliquée dans le plus de contraintes
- **Random** : Sélection aléatoire pour les tests de performance

#### Ordre des valeurs
- **LCV (Least Constraining Value)** : Privilégie les valeurs qui limitent le moins les autres variables
- **Lexicographic** : Ordre naturel des valeurs
- **Random** : Ordre aléatoire des valeurs

### Algorithme de backtracking
Le solveur principal (`src/solver/solver.cpp`) implémente :
- **Recherche en profondeur** avec backtracking intelligent
- **Forward checking** : Vérification anticipée des contraintes
- **Gestion du temps** : Limite de temps configurable
- **Statistiques détaillées** : Nœuds explorés, backtracks, temps d'exécution
- **Support multi-solutions** : Trouve toutes les solutions ou s'arrête à la première

## Compilation

```bash
make                    # Compilation standard
make clean             # Nettoyage
make debug             # Compilation en mode debug
make release           # Compilation optimisée
```

## Paramètres de configuration

Le solveur utilise la structure `SolverParams` définie dans `src/core/params.h` pour gérer tous les paramètres :

### Limites de temps et solutions
- `max_time` (défaut: 300) : Temps maximum en secondes
- `first_solution_only` (défaut: false) : Arrêter à la première solution trouvée

### Stratégies de recherche
- `var_strategy` (défaut: "mrv") : Stratégie de sélection des variables
  - `"mrv"` : Minimum Remaining Values (heuristique optimale)
  - `"degree"` : Sélection par degré de contraintes
  - `"random"` : Sélection aléatoire
- `val_strategy` (défaut: "lcv") : Stratégie de sélection des valeurs
  - `"lcv"` : Least Constraining Value (heuristique optimale)
  - `"random"` : Ordre aléatoire des valeurs
  - `"lexicographic"` : Ordre lexicographique

### Propagation de contraintes
- `use_ac3` (défaut: true) : Utiliser l'algorithme AC-3
- `use_forward_checking` (défaut: true) : Utiliser le forward checking
- `ac3_at_each_node` (défaut: true) : Appliquer AC-3 à chaque nœud de backtracking

### Contrôle de sortie
- `verbose` (défaut: false) : Mode verbeux avec traces détaillées
- `max_depth_trace` (défaut: 5) : Profondeur maximale pour les traces détaillées
- `max_depth_ac3_trace` (défaut: 3) : Profondeur maximale pour les traces AC-3
- `show_global_stats_only` (défaut: false) : Afficher seulement les statistiques globales
- `output_path` (défaut: "") : Chemin de sortie personnalisé

## Utilisation

```bash
./CPSolver <fichier.csp> [options]

Options disponibles:
  -t <time>      Temps maximum en secondes (défaut: 300)
  -f             Arrêter à la première solution trouvée
  -v <strategy>  Stratégie de sélection des variables: mrv, degree, random
  -w <strategy>  Stratégie de sélection des valeurs: lcv, random, lexicographic
  -a             Désactiver complètement AC-3
  -c             Désactiver forward checking
  -n             Désactiver AC-3 à chaque nœud (garder AC-3 initial)
  -o <path>      Chemin de sortie personnalisé
  -V             Mode verbeux (traces détaillées)
  -h             Afficher l'aide complète
```

## Exemples d'utilisation

```bash
# Résolution standard avec toutes les optimisations
./CPSolver ../instances/instances/equality_example.csp

# Première solution seulement (plus rapide)
./CPSolver ../instances/instances/equality_example.csp -f

# Stratégies personnalisées
./CPSolver ../instances/instances/equality_example.csp -v degree -w random

# Désactiver certaines optimisations pour comparaison
./CPSolver ../instances/instances/equality_example.csp -a -c

# Mode verbeux avec traces détaillées
./CPSolver ../instances/instances/equality_example.csp -V

# Limite de temps personnalisée
./CPSolver ../instances/instances/equality_example.csp -t 60

# Sortie personnalisée
./CPSolver ../instances/instances/equality_example.csp -o my_solution.sol

# Configuration complète pour tests de performance
./CPSolver ../instances/instances/equality_example.csp -t 120 -v mrv -w lcv -V -o perf_test.sol
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

## Architecture détaillée

### Composants principaux

#### 1. Parser (src/parser/)
- **parser.h/cpp** : Analyse les fichiers CSP au format DIMACS
- Parse les variables, domaines et contraintes
- Validation de la syntaxe et détection d'erreurs
- Structure de données `CSPInstance` pour représenter le problème

#### 2. Solveur principal (src/solver/)
- **solver.h/cpp** : Classe `CSPSolver` avec l'algorithme de backtracking
- Gestion de l'assignation des variables
- Intégration avec AC-3 et forward checking
- Statistiques de performance (nœuds, backtracks, temps)

#### 3. Algorithmes de consistance (src/algorithms/)
- **ac3.h/cpp** : Classe `AC3Algorithm` pour la propagation de contraintes
- Implémentation de l'algorithme AC-3 avec file de travail
- Gestion des domaines et détection d'inconsistance
- Statistiques de révisions et performance

#### 4. Stratégies de sélection (src/strategies/)
- **strategies.h/cpp** : Classe `SelectionStrategies` pour les heuristiques
- Implémentation MRV, Degree, LCV et stratégies aléatoires
- Calcul des conflits et interactions entre variables
- Génération de nombres aléatoires pour les tests

#### 5. Entrée/Sortie (src/io/)
- **solution_writer.h/cpp** : Écriture des solutions au format texte
- **logo.h** : Interface utilisateur avec logo et affichage formaté
- Gestion des fichiers de sortie et création de répertoires

#### 6. Configuration (src/core/)
- **params.h** : Structure `SolverParams` pour tous les paramètres
- Valeurs par défaut et validation des options
- Configuration centralisée du comportement du solveur

## Performance

Le solveur est optimisé pour :
- Trouver toutes les solutions possibles
- Minimiser l'espace de recherche avec AC-3 et forward checking
- Gérer efficacement les grandes instances
- Fournir des statistiques détaillées