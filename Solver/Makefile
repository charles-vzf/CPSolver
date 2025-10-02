# Makefile pour CPSolver
# Compilateur C++
CXX = g++

# Flags de compilation
# Par défaut, on compile en mode release avec les assertions désactivées (-DNDEBUG)
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g -DNDEBUG

# Nom de l'exécutable
TARGET = CPSolver

# Dossier source
SRCDIR = .

# Dossier d'objets
OBJDIR = obj

# Fichiers sources
SOURCES = main.cpp src/parser/parser.cpp src/solver/solver.cpp src/algorithms/ac3.cpp src/strategies/strategies.cpp src/io/solution_writer.cpp

# Fichiers objets
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# Règle par défaut
all: check_deps $(TARGET)

# Vérification des dépendances
check_deps:
	@echo "Vérification des dépendances..."
	@which $(CXX) > /dev/null || (echo "ERREUR: g++ n'est pas installé. Installez avec: sudo apt-get install g++" && exit 1)
	@echo "g++ trouvé ✓"
	@echo "Dépendances OK ✓"

# Création du dossier obj
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Compilation des fichiers objets
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Lien pour créer l'exécutable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)
	@echo "Compilation terminée avec succès!"
	@echo "Exécutable créé: $(TARGET)"

# Règle pour nettoyer
clean:
	rm -rf $(OBJDIR) $(TARGET)
	@echo "Nettoyage terminé"


# Règle pour exécuter avec un exemple
run: $(TARGET)
	@echo "Exécution du solver avec un exemple..."
	@if [ -f "../instances/example.csp" ]; then \
		./$(TARGET) ../instances/example.csp; \
	else \
		echo "Aucun fichier d'exemple trouvé dans ../instances/"; \
	fi

# Règle pour debug: recompile avec les assertions activées et plus d'infos de debug
debug: clean
	$(MAKE) CXXFLAGS="-std=c++17 -Wall -Wextra -g3"

# Règle pour release
release: CXXFLAGS += -O3 -DNDEBUG
release: clean $(TARGET)

# Aide
help:
	@echo "Commandes disponibles:"
	@echo "  make          - Compile le projet en mode release (rapide)"
	@echo "  make clean    - Nettoie les fichiers générés"
	@echo "  make run      - Exécute avec un exemple"
	@echo "  make debug    - Compile en mode debug (avec assertions)"
	@echo "  make release  - Compile en mode release optimisé"
	@echo "  make help     - Affiche cette aide"

# Déclaration des cibles phony
.PHONY: all clean run debug release help check_deps
