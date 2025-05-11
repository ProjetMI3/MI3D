#include "structures.h"
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
