#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <map>
#include <set>

// Structure pour représenter une contrainte au format DIMACS
struct Constraint {
    int var1;                   // ID de la première variable
    int var2;                   // ID de la deuxième variable
    std::vector<std::pair<int, int>> allowed_pairs; // Paires de valeurs autorisées
    
    Constraint(int v1, int v2) : var1(v1), var2(v2) {}
};

// Structure pour représenter une instance CSP au format DIMACS
struct CSPInstance {
    int num_variables;                              // Nombre de variables
    std::vector<std::pair<int, int>> domains;      // Domaines (min, max) pour chaque variable
    std::vector<Constraint> constraints;           // Contraintes
    
    // Méthodes utilitaires
    bool hasVariable(int var) const;
    std::vector<int> getNeighbors(int var) const;
    std::vector<Constraint> getConstraints(int var) const;
    bool isConsistent(int var1, int val1, int var2, int val2) const;
    std::vector<int> getDomain(int var) const;     // Retourne toutes les valeurs du domaine
};

// Fonction principale de parsing
CSPInstance parseCSPFile(const std::string& filename);

// Fonctions utilitaires de parsing DIMACS
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
bool isComment(const std::string& line);
bool isEmpty(const std::string& line);
std::pair<int, int> parseDomainLine(const std::string& line);
std::vector<std::pair<int, int>> parseConstraintLine(const std::string& line);

#endif // PARSER_H
