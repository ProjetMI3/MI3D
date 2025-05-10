# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g  # Debugging symbols (-g) optional

# Object files
OBJ = main.o fonctions.o jouer_partie.o structures.o

# Executable name
EXEC = cardyard

# Default rule to create the executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

# Compilation of individual source files
main.o: main.c structures.h fonctions.h jouer_partie.h
	$(CC) $(CFLAGS) -c main.c

fonctions.o: fonctions.c structures.h
	$(CC) $(CFLAGS) -c fonctions.c

jouer_partie.o: jouer_partie.c structures.h fonctions.h
	$(CC) $(CFLAGS) -c jouer_partie.c

structures.o: structures.c structures.h
	$(CC) $(CFLAGS) -c structures.c

# Clean up object files and executable
clean:
	rm -f $(OBJ) $(EXEC)

# Force recompilation of everything
rebuild: clean $(EXEC)
