CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = 
SRC = main.c jouer_partie.c fonctions.c
OBJ = $(SRC:.c=.o)
EXEC = cardyard

# Règle par défaut
all: $(EXEC)

# Compilation des fichiers sources
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ > /dev/null 2>&1

# Nettoyage des fichiers objets et exécutables
clean:
	rm -f $(OBJ) $(EXEC)

# Recompile tout (utile pour un nettoyage complet)
fclean: clean
	rm -f $(EXEC)

# Reconstruction du projet (nettoyage et recompilation)
re: fclean all
