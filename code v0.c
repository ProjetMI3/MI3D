#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NB_CARTES_MAX 151
#define NB_CARTES_JOUEUR 6
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
    Carte main[NB_CARTES_JOUEUR];
    Pile defausse;
} Joueur;

Carte creer_carte(int valeur, int visible) {
    Carte c;
    c.valeur = valeur;
    c.visible = visible;
    return c;
}

void melanger_pile(Pile* pile) {
    for (int i = pile->top - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carte temp = pile->cartes[i];
        pile->cartes[i] = pile->cartes[j];
        pile->cartes[j] = temp;
    }
}

void initialiser_pioche(Pile* pioche) {
    pioche->top = 0;
    int quantites[3] = {5, 10, 15};

    for (int valeur = -2; valeur <= 12; valeur++) {
        int nombre = (valeur == -2) ? quantites[0] :
                     (valeur == -1) ? quantites[1] :
                     (valeur == 0)  ? quantites[2] : 10;

        for (int i = 0; i < nombre; i++) {
            Carte nouvelle = creer_carte(valeur, 0);
            pioche->cartes[pioche->top++] = nouvelle;
        }
    }

    melanger_pile(pioche);
}

void initialiser_joueur(Joueur* joueur, const char* nom, Pile* pioche) {
    joueur->defausse.top = 0;

    for (int i = 0; i < NB_CARTES_JOUEUR; i++) {
        if (pioche->top == 0) {
            printf("Erreur : plus de cartes dans la pioche.\n");
            exit(1);
        }
        joueur->main[i] = pioche->cartes[--pioche->top];
        joueur->main[i].visible = 0;
    }
}

void afficher_carte(Carte c) {
    if (c.visible)
        printf("[ %2d ]", c.valeur);
    else
        printf("[ ?? ]");
}

void afficher_main(Joueur joueur) {
    for (int i = 0; i < NB_CARTES_JOUEUR; i++) {
        printf("%d: ", i);
        afficher_carte(joueur.main[i]);
        printf(" ");
    }
    printf("\n");
}

void jouer_partie(Joueur joueurs[], int nb_joueurs, Pile *pioche) {
    int tour = 0, fin = 0, premier_termine = -1;

    while (!fin) {
        Joueur *actif = &joueurs[tour];

        printf("\n--- Tour de %s ---\n", actif->nom);
        printf("Cartes dans la pioche centrale : %d\n", pioche->top);

        if (actif->defausse.top > 0) {
            printf("Sommet de votre défausse : ");
            afficher_carte(actif->defausse.cartes[actif->defausse.top - 1]);
            printf("\n");
        }

        int choix_source;
        printf("1: piocher dans la pioche centrale, 2: prendre votre défausse\n> ");
        scanf("%d", &choix_source);

        Carte piochee;
        if (choix_source == 1) {
            if (pioche->top == 0) {
                printf("La pioche est vide ! Tour annulé.\n");
                tour = (tour + 1) % nb_joueurs;
                continue;
            }
            piochee = pioche->cartes[--pioche->top];
            piochee.visible = 1;
        } else {
            if (actif->defausse.top == 0) {
                printf("Défausse vide ! Tour annulé.\n");
                tour = (tour + 1) % nb_joueurs;
                continue;
            }
            piochee = actif->defausse.cartes[--actif->defausse.top];
        }

        afficher_main(*actif);
        int idx;
        printf("Index de la carte à échanger (0 à %d) : ", NB_CARTES_JOUEUR - 1);
        scanf("%d", &idx);
        while (idx < 0 || idx >= NB_CARTES_JOUEUR) {
            printf("Index invalide, ressaisissez : ");
            scanf("%d", &idx);
        }

        Carte remplacee = actif->main[idx];
        remplacee.visible = 1;
        actif->defausse.cartes[actif->defausse.top++] = remplacee;
        actif->main[idx] = piochee;

        int toutes_visibles = 1;
        for (int i = 0; i < NB_CARTES_JOUEUR; i++) {
            if (!actif->main[i].visible) {
                toutes_visibles = 0;
                break;
            }
        }

        if (toutes_visibles) {
            if (premier_termine < 0) {
                premier_termine = tour;
            } else if (tour == premier_termine) {
                fin = 1;
            }
        }

        tour = (tour + 1) % nb_joueurs;
    }

    printf("\n--- Fin de la partie ---\n");
    for (int i = 0; i < nb_joueurs; i++) {
        int score = 0;
        for (int j = 0; j < NB_CARTES_JOUEUR; j++)
            score += joueurs[i].main[j].valeur;
        printf("%s : %d points\n", joueurs[i].nom, score);
    }
}

int main() {
    srand(time(NULL));
    int nb_joueurs;

    printf("Bienvenue sur CardYard (version française) !\n");
    printf("Combien de joueurs ? (2 à %d) : ", NB_JOUEURS_MAX);
    if (scanf("%d", &nb_joueurs) != 1 || nb_joueurs < 2 || nb_joueurs > NB_JOUEURS_MAX) {
        fprintf(stderr, "Entrée invalide.\n");
        return 1;
    }

    Joueur joueurs[NB_JOUEURS_MAX];
    Pile pioche;
    initialiser_pioche(&pioche);

    for (int i = 0; i < nb_joueurs; i++) {
        char nom_temp[256];
        printf("Nom du joueur %d : ", i + 1);
        scanf("%255s", nom_temp);
        joueurs[i].nom = malloc(strlen(nom_temp) + 1);
        if (!joueurs[i].nom) {
            fprintf(stderr, "Erreur d'allocation mémoire.\n");
            exit(1);
        }
        strcpy(joueurs[i].nom, nom_temp);
        initialiser_joueur(&joueurs[i], nom_temp, &pioche);
    }

    jouer_partie(joueurs, nb_joueurs, &pioche);

    for (int i = 0; i < nb_joueurs; i++) {
        free(joueurs[i].nom);
    }

    return 0;
}
