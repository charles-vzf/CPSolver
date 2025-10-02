#ifndef AC3_H
#define AC3_H

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include "../parser/parser.h"

// Structure pour représenter un arc
struct Arc {
    int var1;
    int var2;
    
    Arc(int v1, int v2) : var1(v1), var2(v2) {}
    
    bool operator==(const Arc& other) const {
        return var1 == other.var1 && var2 == other.var2;
    }
};

// Classe pour l'algorithme AC-3
class AC3Algorithm {
private:
    CSPInstance csp;
    std::vector<std::vector<int>> domains;
    std::queue<Arc> worklist;
    int revisions_count;
    
    // Méthodes privées
    bool revise(int var1, int var2);
    std::vector<Arc> getArcs(int var) const;
    bool isConsistent(int var1, int val1, int var2, int val2) const;
    
public:
    AC3Algorithm(const CSPInstance& instance);
    
    // Appliquer AC-3 et retourner true si l'instance est consistante
    bool apply(bool verbose = true);
    
    // Initialiser avec des domaines spécifiques
    void setDomains(const std::vector<std::vector<int>>& new_domains);
    
    // Obtenir les domaines après AC-3
    std::vector<std::vector<int>> getDomains() const;
    
    // Obtenir le nombre de révisions effectuées
    int getRevisionsCount() const;
    
    // Vérifier si un domaine est vide
    bool hasEmptyDomain() const;
    
    // Obtenir les statistiques
    void printStats() const;
};

#endif // AC3_H
