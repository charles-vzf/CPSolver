#!/usr/bin/env python3
"""
Générateur de problèmes CSP pour le problème des n-reines
Usage: python generate_nqueens.py <n> [output_dir]
"""

import sys
import os
import argparse
from pathlib import Path

def generate_nqueens_csp(n, output_dir="instances"):
    """
    Génère un fichier CSP pour le problème des n-reines
    
    Args:
        n (int): Taille de l'échiquier (nombre de reines)
        output_dir (str): Répertoire de sortie
    """
    
    if n < 4:
        raise ValueError("Le problème des n-reines n'a pas de solution pour n < 4")
    
    # Créer le répertoire de sortie s'il n'existe pas
    os.makedirs(output_dir, exist_ok=True)
    
    # Nom du fichier de sortie
    filename = f"nqueens_{n}.csp"
    filepath = os.path.join(output_dir, filename)
    
    with open(filepath, 'w') as f:
        # En-tête du fichier
        f.write("# CSP pour le problème des n-reines\n")
        f.write(f"# Échiquier {n}x{n}, {n} reines à placer\n")
        f.write(f"# Variables: {n} (une par ligne)\n")
        f.write(f"# Domaines: [1, {n}] (positions des colonnes)\n")
        f.write(f"# Contraintes: pas d'attaque entre reines\n")
        f.write("\n")
        
        # Nombre de variables
        f.write(f"{n}\n")
        f.write("\n")
        
        # Domaines des variables (toutes les variables ont le même domaine [1, n])
        f.write("# Variable domains (variable_id min_value max_value)\n")
        for i in range(n):
            f.write(f"{i} 1 {n}\n")
        f.write("\n")
        
        # Calculer le nombre de contraintes
        # Pour chaque paire de reines (i,j) avec i < j, on a 3 contraintes:
        # 1. Pas sur la même colonne: col_i != col_j
        # 2. Pas sur la même diagonale montante: i - col_i != j - col_j
        # 3. Pas sur la même diagonale descendante: i + col_i != j + col_j
        num_constraints = 3 * (n * (n - 1) // 2)  # 3 contraintes par paire
        
        f.write(f"{num_constraints}\n")
        f.write("\n")
        
        # Écrire les contraintes
        f.write("# Constraints (var1 var2 (value1,value2) (value3,value4) ...)\n")
        
        for i in range(n):
            for j in range(i + 1, n):
                # Contrainte 1: Pas sur la même colonne
                # Toutes les paires (col_i, col_j) où col_i != col_j
                allowed_pairs = []
                for col_i in range(1, n + 1):
                    for col_j in range(1, n + 1):
                        if col_i != col_j:
                            allowed_pairs.append((col_i, col_j))
                
                f.write(f"{i} {j}")
                for pair in allowed_pairs:
                    f.write(f" ({pair[0]},{pair[1]})")
                f.write("\n")
                
                # Contrainte 2: Pas sur la même diagonale montante
                # Deux reines sont sur la même diagonale montante si i - col_i = j - col_j
                # Donc on interdit les paires où i - col_i = j - col_j
                allowed_pairs = []
                for col_i in range(1, n + 1):
                    for col_j in range(1, n + 1):
                        if i - col_i != j - col_j:
                            allowed_pairs.append((col_i, col_j))
                
                f.write(f"{i} {j}")
                for pair in allowed_pairs:
                    f.write(f" ({pair[0]},{pair[1]})")
                f.write("\n")
                
                # Contrainte 3: Pas sur la même diagonale descendante
                # Deux reines sont sur la même diagonale descendante si i + col_i = j + col_j
                # Donc on interdit les paires où i + col_i = j + col_j
                allowed_pairs = []
                for col_i in range(1, n + 1):
                    for col_j in range(1, n + 1):
                        if i + col_i != j + col_j:
                            allowed_pairs.append((col_i, col_j))
                
                f.write(f"{i} {j}")
                for pair in allowed_pairs:
                    f.write(f" ({pair[0]},{pair[1]})")
                f.write("\n")
    
    print(f"Fichier CSP généré: {filepath}")
    print(f"Variables: {n}")
    print(f"Contraintes: {num_constraints}")
    return filepath

def main():
    parser = argparse.ArgumentParser(description='Générateur de problèmes CSP pour n-reines')
    parser.add_argument('n', type=int, help='Nombre de reines (taille de l\'échiquier)')
    parser.add_argument('-o', '--output', default='instances', 
                       help='Répertoire de sortie (défaut: instances)')
    parser.add_argument('--range', action='store_true',
                       help='Générer pour toutes les tailles de 4 à n')
    
    args = parser.parse_args()
    
    if args.range:
        # Générer pour toutes les tailles de 4 à n
        for size in range(4, args.n + 1):
            try:
                generate_nqueens_csp(size, args.output)
            except ValueError as e:
                print(f"Erreur pour n={size}: {e}")
    else:
        # Générer pour une seule taille
        try:
            generate_nqueens_csp(args.n, args.output)
        except ValueError as e:
            print(f"Erreur: {e}")
            sys.exit(1)

if __name__ == "__main__":
    main()
