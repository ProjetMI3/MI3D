#include "structures.h"
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

void initialiser_joueur(Joueur* joueur, const char* nom, Pile* pioche, int nb_cartes) {
    joueur->defausse.top = 0;
    joueur->main = malloc(nb_cartes * sizeof(Carte)); // Allocation dynamique pour les cartes du joueur

    if (!joueur->main) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        exit(1);
    }

    for (int i = 0; i < nb_cartes; i++) {
        if (pioche->top == 0) {
            printf("Erreur : plus de cartes dans la pioche.\n");
            exit(1);
        }
        joueur->main[i] = pioche->cartes[--pioche->top];
        joueur->main[i].visible = 0;
    }
}

void afficher_carte(Carte c) {
    if (!c.visible) {
        // Carte non visible (face cachée)
        printf("\033[1;30;100m[ ?? ]\033[0m ");
        return;
    }

    int val = c.valeur;
    const char* color_code;

    if (val == -2)        color_code = "\033[1;37;41m";   // texte blanc, fond rouge
    else if (val <= 0)    color_code = "\033[1;30;43m";   // texte noir, fond jaune
    else if (val <= 5)    color_code = "\033[1;30;42m";   // texte noir, fond vert
    else if (val <= 8)    color_code = "\033[1;30;46m";   // texte noir, fond cyan
    else                  color_code = "\033[1;37;45m";   // texte blanc, fond magenta

    printf("%s[ %2d ]\033[0m ", color_code, val);
}

void afficher_main(Joueur joueur, int nb_cartes) {
    for (int i = 0; i < nb_cartes; i++) {
        printf("%d: ", i);
        afficher_carte(joueur.main[i]);
        printf(" ");
    }
    printf("\n");
}

void afficher_defausse(Joueur joueur) {
    if (joueur.defausse.top > 0) {
        printf("Défausse de %s : ", joueur.nom);
        for (int i = 0; i < joueur.defausse.top; i++) {
            afficher_carte(joueur.defausse.cartes[i]);
            printf(" ");
        }
        printf("\n");
    }
}
