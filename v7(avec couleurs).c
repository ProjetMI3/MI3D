#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NB_CARTES_MAX 150
#define NB_JOUEURS_MAX 8

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
        int nombre;
        if (valeur == -2) {
            nombre = quantites[0];
        } 
        else if (valeur == -1) {
            nombre = quantites[1];
        } 
        else if (valeur == 0) {
            nombre = quantites[2];
        } else {
            nombre = 10;
        }


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

void jouer_partie(Joueur joueurs[], int nb_joueurs, Pile *pioche, int nb_cartes) {
    int tour = 0, fin = 0, premier_termine = -1;
    int tour_total = 0;

    while (!fin) {
        Joueur *actif = &joueurs[tour];

        printf("\n--- Tour de %s ---\n", actif->nom);
        printf("Cartes dans la pioche centrale : %d\n", pioche->top);

        // Affichage conditionnel des défausses des autres joueurs (si au moins une existe et pas le 1er tour)
        int defausse_affichee = 0;
        if (tour_total > 0) {
            for (int i = 0; i < nb_joueurs; i++) {
                if (i != tour && joueurs[i].defausse.top > 0) {
                    if (!defausse_affichee) {
                        printf("\nDéfausses des autres joueurs :\n");
                        defausse_affichee = 1;
                    }
                    afficher_defausse(joueurs[i]);
                }
            }
        }

        // Affichage de la propre défausse (si elle existe)
        if (actif->defausse.top > 0) {
            afficher_defausse(*actif);
        }

        // Affichage de la main du joueur
        afficher_main(*actif, nb_cartes);

        // Choix de la source
        int choix_source;
        while (1) {  // Boucle pour assurer une saisie valide
            printf("\n1: piocher dans la pioche centrale\n");
            printf("2: piocher dans votre propre défausse\n");
            if (tour_total > 0) {
                // Vérifie qu’au moins une autre défausse est disponible
                int defausse_disponible = 0;
                for (int i = 0; i < nb_joueurs; i++) {
                    if (i != tour && joueurs[i].defausse.top > 0) {
                        defausse_disponible = 1;
                        break;
                    }
                }
                if (defausse_disponible) {
                    printf("3: piocher dans la défausse d'un autre joueur\n");
                }
            }

            printf("> ");
            if (scanf("%d", &choix_source) != 1) {
                while (getchar() != '\n');  // Vider le buffer de saisie
                printf("Saisie invalide. Essayez encore.\n");
                continue;  // Redemander la saisie
            }

            // Vérification de la saisie du joueur
            if (choix_source == 1 || (choix_source == 2 && actif->defausse.top > 0) || (choix_source == 3 && tour_total > 0)) {
                break;  // Saisie valide, sortir de la boucle
            } else {
                printf("Choix invalide, ressaisissez : ");
            }
        }

        Carte piochee;
        int joueur_choisi = -1;  // Initialisation ici

        if (choix_source == 1) {
            if (pioche->top == 0) {
                printf("La pioche est vide ! Tour annulé.\n");
                tour = (tour + 1) % nb_joueurs;
                tour_total++;
                continue;
            }
            piochee = pioche->cartes[--pioche->top];
            piochee.visible = 1;
            printf("Vous avez pioché : ");
            afficher_carte(piochee);
            printf("\n");

        } else if (choix_source == 2) {
            // Vérifier si la défausse du joueur est vide
            if (actif->defausse.top == 0) {
                printf("Vous n'avez pas de cartes dans votre défausse !\n");
                continue;  // Redemander le choix sans annuler le tour
            }

            // Piocher dans la propre défausse
            piochee = actif->defausse.cartes[--actif->defausse.top];
            printf("Vous avez pris une carte de votre défausse : ");
            afficher_carte(piochee);
            printf("\n");

        } else if (choix_source == 3 && tour_total > 0) {
            // Afficher uniquement les joueurs valides
            printf("Joueurs disponibles pour piocher leur défausse :\n");
            for (int i = 0; i < nb_joueurs; i++) {
                if (i != tour && joueurs[i].defausse.top > 0) {
                    printf("%d: %s\n", i, joueurs[i].nom);
                }
            }

            printf("Numéro du joueur : ");
            if (scanf("%d", &joueur_choisi) != 1 || joueur_choisi < 0 || joueur_choisi >= nb_joueurs || joueur_choisi == tour || joueurs[joueur_choisi].defausse.top == 0) {
                printf("Choix invalide. Vous devez piocher dans la pioche centrale.\n");
                piochee = pioche->cartes[--pioche->top];
                piochee.visible = 1;
                printf("Vous avez pioché : ");
                afficher_carte(piochee);
                printf("\n");
                joueur_choisi = -1;  // Pour éviter de modifier sa défausse
            } else {
                piochee = joueurs[joueur_choisi].defausse.cartes[--joueurs[joueur_choisi].defausse.top];
                printf("Vous avez pris une carte de la défausse de %s : ", joueurs[joueur_choisi].nom);
                afficher_carte(piochee);
                printf("\n");
            }
        }

        // Choix de la carte à remplacer
        printf("\nVotre main :\n");
    afficher_main(*actif, nb_cartes);

    int idx;
    while (1) {
        printf("Index de la carte à échanger (0 à %d) : ", nb_cartes - 1);  // Affichage ici (une seule fois)
        if (scanf("%d", &idx) == 1) {
            if (idx >= 0 && idx < nb_cartes) {
                break;  // sortie : saisie correcte et dans les limites
            } else {
                printf("Index hors limites. Essayez encore.\n");
            }
        } else {
            // Saisie invalide (pas un entier)
            while (getchar() != '\n'); // vide le buffer
            printf("Saisie invalide. Essayez encore.\n");
        }
}


        Carte remplacee = actif->main[idx];
        remplacee.visible = 1;

        // Mise à jour de la défausse du joueur actif
        if (joueur_choisi != -1) {
            joueurs[joueur_choisi].defausse.cartes[joueurs[joueur_choisi].defausse.top++] = remplacee;
        } else {
            actif->defausse.cartes[actif->defausse.top++] = remplacee;
        }

        // Mise à jour de la main du joueur actif
        actif->main[idx] = piochee;

        printf("Votre main mise à jour :\n");
        afficher_main(*actif, nb_cartes);
        printf("Votre défausse mise à jour :\n");
        afficher_defausse(*actif);

        // Fin ?
        int toutes_visibles = 1;
        for (int i = 0; i < nb_cartes; i++) {
            if (!actif->main[i].visible) {
                toutes_visibles = 0;
                break;
            }
        }

        // Vérifie si TOUS les joueurs ont toutes leurs cartes visibles
        int tous_visibles = 1;
        for (int j = 0; j < nb_joueurs; j++) {
            for (int k = 0; k < nb_cartes; k++) {
                if (!joueurs[j].main[k].visible) {
                    tous_visibles = 0;
                    break;
        }
    }
        if (!tous_visibles) break;
}
if (tous_visibles) {
    fin = 1;
}


        tour = (tour + 1) % nb_joueurs;
        tour_total++;
    }
// Résultat
printf("\n--- Fin de la partie ---\n");
int scores[NB_JOUEURS_MAX];
int min_score = 9999; // Une valeur suffisamment grande

// Calcul des scores
for (int i = 0; i < nb_joueurs; i++) {
    int score = 0;
    for (int j = 0; j < nb_cartes; j++)
        score += joueurs[i].main[j].valeur;
    scores[i] = score;
    if (score < min_score) {
        min_score = score;
    }
}

// Tri des scores dans l'ordre croissant
for (int i = 0; i < nb_joueurs - 1; i++) {
    for (int j = 0; j < nb_joueurs - i - 1; j++) {
        if (scores[j] > scores[j + 1]) {
            // Échange des scores
            int temp = scores[j];
            scores[j] = scores[j + 1];
            scores[j + 1] = temp;

            // Échange des noms des joueurs correspondants
            char *temp_nom = joueurs[j].nom;
            joueurs[j].nom = joueurs[j + 1].nom;
            joueurs[j + 1].nom = temp_nom;
        }
    }
}

// Affichage des scores triés dans l'ordre croissant
for (int i = 0; i < nb_joueurs; i++) {
    printf("%s : %d points\n", joueurs[i].nom, scores[i]);
}


// Affichage du/des gagnant(s)
printf("\n🏆 Vainqueur%s : ", (min_score == 9999 ? "" : " "));
int gagnant_unique = 1;
for (int i = 0; i < nb_joueurs; i++) {
    if (scores[i] == min_score) {
        printf("%s ", joueurs[i].nom);
    }
}
printf("avec %d points !\n", min_score);
}


int main() {
    srand(time(NULL));
    int nb_joueurs, nb_cartes;

    printf("Bienvenue sur CardYard (version française) !\n");
    printf("VARIANTE ACTIVE : Nombre de cartes personnelles défini par l'utilisateur (CARD_USER).\n\n");

    // Demande du nombre de joueurs
    do {
        printf("Entrez le nombre de joueurs (entre 2 et 8) : ");
        if (scanf("%d", &nb_joueurs) != 1) {
            while (getchar() != '\n'); // vide le buffer
            nb_joueurs = -1; // force la répétition
        }

        if (nb_joueurs < 2 || nb_joueurs > 8) {
            fprintf(stderr, "Entrée invalide.\n");
        }
    } while (nb_joueurs < 2 || nb_joueurs > 8);

    // Variante CARD_USER : Choix du nombre de cartes personnelles
        // Variante CARD_USER : Choix du nombre de cartes personnelles
    do {
        printf("Combien de cartes personnelles voulez-vous distribuer à chaque joueur ? (1 à 10) : ");
        if (scanf("%d", &nb_cartes) != 1) {
            while (getchar() != '\n'); // vide le buffer en cas d'échec
            nb_cartes = -1;            // force la répétition
            fprintf(stderr, "Entrée invalide.\n");
            continue;
        }
        if (nb_cartes < 1 || nb_cartes > 10) {
            fprintf(stderr, "Entrée invalide.\n");
        }
    } while (nb_cartes < 1 || nb_cartes > 10);

    // ── VIDAGE DU '\n' RESTANT APRÈS LE scanf
    {
        int _ch;
        while ((_ch = getchar()) != '\n' && _ch != EOF);
    }

    // Initialisation des joueurs et de la pioche
    Joueur joueurs[NB_JOUEURS_MAX];
    Pile pioche;
    initialiser_pioche(&pioche);

    // Boucle de saisie des noms
    for (int i = 0; i < nb_joueurs; i++) {
        char nom_temp[256];
        printf("Nom du joueur %d : ", i + 1);
        if (!fgets(nom_temp, sizeof(nom_temp), stdin)) {
            fprintf(stderr, "Erreur de lecture.\n");
            i--;
            continue;
        }

        // Supprime le '\n' final s’il existe
        nom_temp[strcspn(nom_temp, "\n")] = '\0';

        // Chaîne vide ?
        if (nom_temp[0] == '\0') {
            printf("Nom vide non autorisé. Réessayez.\n");
            i--;
            continue;
        }

        // Vérification manuelle : chaque caractère doit être A–Z ou a–z
        int valide = 1;
        for (size_t k = 0; k < strlen(nom_temp); k++) {
            char c = nom_temp[k];
            if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == ' '))) {
                valide = 0;
                break;
            }
        }
        if (!valide) {
            printf("Nom invalide : n'entrez que des lettres (A–Z, a–z).\n");
            i--;
            continue;
        }

        // Allocation et copie du nom validé
        joueurs[i].nom = malloc(strlen(nom_temp) + 1);
        if (!joueurs[i].nom) {
            fprintf(stderr, "Erreur d'allocation mémoire.\n");
            exit(1);
        }
        strcpy(joueurs[i].nom, nom_temp);
        initialiser_joueur(&joueurs[i], joueurs[i].nom, &pioche, nb_cartes);
    }


    jouer_partie(joueurs, nb_joueurs, &pioche, nb_cartes);

    for (int i = 0; i < nb_joueurs; i++) {
        free(joueurs[i].nom);
        free(joueurs[i].main); // Libération de la mémoire allouée pour les cartes
    }

    return 0;
}
