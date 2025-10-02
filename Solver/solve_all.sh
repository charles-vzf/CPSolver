#!/bin/bash

# Script pour exécuter CPSolver sur toutes les instances
# Usage: ./solve_all.sh

# Configuration
INSTANCES_DIR="../instances/instances"
SOLUTIONS_DIR="../solutions/solutions"
TRACE_FILE="trace_solve_all.txt"
SOLVER="./CPSolver"

# Fonction pour afficher les messages avec timestamp
log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Fonction pour vérifier les dépendances
check_dependencies() {
    log_message "Vérification des dépendances..."
    
    # Vérifier que make est disponible
    if ! command -v make > /dev/null 2>&1; then
        log_message "✗ Erreur: 'make' n'est pas installé"
        return 1
    fi
    
    # Vérifier que g++ est disponible
    if ! command -v g++ > /dev/null 2>&1; then
        log_message "✗ Erreur: 'g++' n'est pas installé"
        log_message "Installez avec: sudo apt-get install g++"
        return 1
    fi
    
    # Vérifier que le Makefile existe
    if [ ! -f "Makefile" ]; then
        log_message "✗ Erreur: Makefile introuvable"
        return 1
    fi
    
    log_message "✓ Dépendances OK"
    return 0
}

# Fonction pour nettoyer et compiler
compile_solver() {
    log_message "Début de la compilation du solveur..."
    
    # Vérifier les dépendances
    if ! check_dependencies; then
        return 1
    fi
    
    # Nettoyage
    log_message "Nettoyage des fichiers générés..."
    if make clean > /dev/null 2>&1; then
        log_message "✓ Nettoyage réussi"
    else
        log_message "⚠ Avertissement: Erreur lors du nettoyage (peut être normal si rien à nettoyer)"
    fi
    
    # Compilation
    log_message "Compilation du solveur..."
    if make > /dev/null 2>&1; then
        log_message "✓ Compilation réussie"
        return 0
    else
        log_message "✗ Erreur lors de la compilation"
        log_message "Détails de l'erreur:"
        make 2>&1 | head -20
        return 1
    fi
}

if ! compile_solver; then
    log_message "Erreur: Impossible de compiler CPSolver. Vérifiez les dépendances et le code source."
    exit 1
fi

# Vérifier que le répertoire des instances existe
if [ ! -d "$INSTANCES_DIR" ]; then
    echo "Erreur: Le répertoire des instances n'existe pas: $INSTANCES_DIR"
    exit 1
fi

# Créer le répertoire des solutions s'il n'existe pas
mkdir -p "$SOLUTIONS_DIR"

# Initialiser le fichier de trace
echo "=== TRACE DE RÉSOLUTION CPSolver ===" > "$TRACE_FILE"
echo "Date de début: $(date)" >> "$TRACE_FILE"
echo "Répertoire des instances: $INSTANCES_DIR" >> "$TRACE_FILE"
echo "Répertoire des solutions: $SOLUTIONS_DIR" >> "$TRACE_FILE"
echo "Mode de compilation: Systématique (make clean + make)" >> "$TRACE_FILE"
echo "" >> "$TRACE_FILE"

# Compteurs
total_instances=0
successful_solves=0
no_solution_found=0
failed_solves=0
error_list=()

# Parcourir toutes les instances .csp
for instance_file in "$INSTANCES_DIR"/*.csp; do
    # Vérifier que le fichier existe (au cas où il n'y aurait pas de .csp)
    if [ ! -f "$instance_file" ]; then
        continue
    fi
    
    # Extraire le nom de base du fichier (sans extension et sans chemin)
    instance_name=$(basename "$instance_file" .csp)
    solution_file="$SOLUTIONS_DIR/${instance_name}.sol"
    
    total_instances=$((total_instances + 1))
    
    echo "Traitement de l'instance: $instance_name" | tee -a "$TRACE_FILE"
    echo "Fichier d'instance: $instance_file" >> "$TRACE_FILE"
    echo "Fichier de solution: $solution_file" >> "$TRACE_FILE"
    echo "Début: $(date)" >> "$TRACE_FILE"
    echo "----------------------------------------" >> "$TRACE_FILE"
    
    # Exécuter CPSolver et capturer la sortie dans une variable temporaire
    # Le solveur écrit directement dans le fichier de solution
    solver_output=$(timeout 300 "$SOLVER" "$instance_file" 2>&1)
    solver_exit_code=$?
    
    # Ajouter la sortie du solver au trace
    echo "$solver_output" >> "$TRACE_FILE"
    
    if [ $solver_exit_code -eq 0 ]; then
        # Vérifier si une solution a été trouvée en regardant la sortie du solver ET l'existence du fichier
        if echo "$solver_output" | grep -q "Solutions found:" && ! echo "$solver_output" | grep -q "No solution found" && [ -f "$solution_file" ]; then
            echo "✓ Succès: Solution trouvée pour $instance_name" | tee -a "$TRACE_FILE"
            successful_solves=$((successful_solves + 1))
        else
            echo "ℹ Info: Aucune solution trouvée pour $instance_name (instance peut-être insatisfiable)" | tee -a "$TRACE_FILE"
            no_solution_found=$((no_solution_found + 1))
        fi
    else
        error_msg=""
        if [ $solver_exit_code -eq 124 ]; then
            error_msg="Timeout (5 minutes) pour $instance_name"
            echo "✗ Échec: $error_msg" | tee -a "$TRACE_FILE"
        else
            error_msg="Erreur lors de la résolution de $instance_name (code de sortie: $solver_exit_code)"
            echo "✗ Échec: $error_msg" | tee -a "$TRACE_FILE"
        fi
        failed_solves=$((failed_solves + 1))
        error_list+=("$error_msg")
    fi
    
    echo "Fin: $(date)" >> "$TRACE_FILE"
    echo "" >> "$TRACE_FILE"
done

# Résumé final
echo "========================================" >> "$TRACE_FILE"
echo "RÉSUMÉ FINAL" >> "$TRACE_FILE"
echo "Date de fin: $(date)" >> "$TRACE_FILE"
echo "Mode de compilation: Systématique (make clean + make)" >> "$TRACE_FILE"
echo "Total d'instances traitées: $total_instances" >> "$TRACE_FILE"
echo "Résolutions réussies: $successful_solves" >> "$TRACE_FILE"
echo "Instances sans solution: $no_solution_found" >> "$TRACE_FILE"
echo "Résolutions échouées (erreurs): $failed_solves" >> "$TRACE_FILE"

# Afficher les erreurs s'il y en a
if [ ${#error_list[@]} -gt 0 ]; then
    echo "Liste des erreurs:" >> "$TRACE_FILE"
    for error in "${error_list[@]}"; do
        echo "  - $error" >> "$TRACE_FILE"
    done
fi

echo "Taux de succès: $(( (successful_solves * 100) / total_instances ))%" >> "$TRACE_FILE"

# Afficher le résumé à l'écran
echo ""
echo "========================================"
echo "RÉSUMÉ FINAL"
echo "Mode de compilation: Systématique (make clean + make)"
echo "Total d'instances traitées: $total_instances"
echo "Résolutions réussies: $successful_solves"
echo "Instances sans solution: $no_solution_found"
echo "Résolutions échouées (erreurs): $failed_solves"

# Afficher les erreurs s'il y en a
if [ ${#error_list[@]} -gt 0 ]; then
    echo ""
    echo "Liste des erreurs:"
    for error in "${error_list[@]}"; do
        echo "  - $error"
    done
fi

echo "Taux de succès: $(( (successful_solves * 100) / total_instances ))%"
echo ""
echo "Trace complète sauvegardée dans: $TRACE_FILE"
echo "Solutions sauvegardées dans: $SOLUTIONS_DIR"

# Afficher les informations de compilation
echo ""
echo "Informations de compilation:"
if [ -f "$SOLVER" ]; then
    echo "  - Exécutable: $SOLVER"
    echo "  - Taille: $(du -h "$SOLVER" | cut -f1)"
    echo "  - Date de modification: $(stat -c %y "$SOLVER" 2>/dev/null || stat -f %Sm "$SOLVER" 2>/dev/null || echo "Inconnue")"
fi
