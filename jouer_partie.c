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
        printf("Index de la carte à échanger (0 à %d) : ", nb_cartes - 1);
        while (1) {
    printf("Index de la carte à échanger (0 à %d) : ", nb_cartes - 1);
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

    // Résultats
printf("\n--- Fin de la partie ---\n");
int scores[NB_JOUEURS_MAX];
int min_score = 9999; // Une valeur suffisamment grande

for (int i = 0; i < nb_joueurs; i++) {
    int score = 0;
    for (int j = 0; j < nb_cartes; j++)
        score += joueurs[i].main[j].valeur;
    scores[i] = score;
    if (score < min_score) {
        min_score = score;
    }
    printf("%s : %d points\n", joueurs[i].nom, score);
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
