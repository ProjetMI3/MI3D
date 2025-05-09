# Définir les variables pour la compilation
CC = gcc
CFLAGS = -Wall -Wextra -g  # -g pour les infos de débogage (optionnel)
OBJ = main.o fonctions.o jouer_partie.o structures.o  # Fichiers objets à générer

# Nom de l'exécutable
EXEC = cardyard

# Règle par défaut : compilation de l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

# Règles pour la compilation des fichiers source
main.o: main.c structures.h fonctions.h jouer_partie.h
	$(CC) $(CFLAGS) -c main.c

fonctions.o: fonctions.c structures.h
	$(CC) $(CFLAGS) -c fonctions.c

jouer_partie.o: jouer_partie.c structures.h fonctions.h
	$(CC) $(CFLAGS) -c jouer_partie.c

structures.o: structures.c structures.h
	$(CC) $(CFLAGS) -c structures.c

# Règle pour nettoyer les fichiers objets et l'exécutable
clean:
	rm -f $(OBJ) $(EXEC)

# Règle pour forcer la recompilation de tout
rebuild: clean $(EXEC)
