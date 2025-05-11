#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NB_CARTES_MAX 151
#define NB_JOUEURS_MAX 9

typedef struct {
    int valeur;
    int visible;
} Carte;

typedef struct {
    Carte cartes[NB_CARTES_MAX];
    int top;
} Pile;

typedef struct {
    char *nom;
    Carte *main;  // Utilisation d'un tableau dynamique pour les cartes du joueur
    Pile defausse;
} Joueur;
