/************************************************************************
  Nom du fichier : arbre_prediction.c
  Description : Déclaration du module pour l'arbre de prédiction.
  Auteur : Nachid Ayman
************************************************************************/

// #define NDEBUG
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sequence.h"
#include "tableau_dyn.h"
#include "arbre_prediction.h"

// Création d'un nœud
struct Noeud *creerNoeud_AP(char *mot) {
    // Alloue un nouveau nœud pour l'arbre
    struct Noeud *mon_noeud = (struct Noeud *)malloc(sizeof(struct Noeud));
    if (mon_noeud == NULL) {
        perror("Erreur : mon_noeud est NULL - creer_noeud_AP().\n");
        return NULL;
    }

    // Vérifie que le mot n'est pas NULL
    if (mot == NULL) {
        perror("Erreur : mot est NULL - creer_noeud_AP().\n");
        free(mon_noeud);
        return NULL;
    }

    // Initialise le nœud avec le mot, les occurrences, et les fils
    mon_noeud->mot = mot;  // Utilisation directe du pointeur mot
    mon_noeud->occurrences = 1;
    mon_noeud->fils = creer_TabD(2, 2);  // Crée un tableau dynamique pour les fils

    // Vérifie que le tableau dynamique des fils a été correctement créé
    if (mon_noeud->fils == NULL) {
        perror("Erreur : mon_noeud->fils est NULL - creer_noeud_AP().\n");
        free(mon_noeud);
        return NULL;
    }

    return mon_noeud;
}

// Création de la racine de l'arbre
struct Noeud *creerRacine_AP() {
    // Crée un nœud racine avec un mot vide
    struct Noeud *racine = creerNoeud_AP("");  
    return racine;
}

// Recherche ou complète un chemin N-gramme dans l'arbre
struct Noeud *rechercher_completer_ngramme_AP(struct Noeud *racine) {
    if (racine == NULL) {
        perror("Erreur : racine est NULL. - rechercher_ngramme_AP()\n");
        return NULL;
    }

    struct Noeud *mon_noeud_courant = racine;  // Commence à partir de la racine
    int index;

    // Initialise l'itérateur pour parcourir la séquence
    sequence_itStart();

    // Parcourt les mots du N-gramme
    while (sequence_itHasNext()) {
        void *mot = (void *)sequence_itNext();

        // Recherche le mot dans les fils du nœud courant
        index = rechercheElement_TabD(comparerMot_AP, mot, mon_noeud_courant->fils);

        if (index != -1) {
            // Si le mot est trouvé, passe au nœud correspondant
            mon_noeud_courant = (struct Noeud *)lireElement_TabD(mon_noeud_courant->fils, index);
        } else {
            // Sinon, ajoute un nouveau nœud pour ce mot
            ajouterMot_AP(mon_noeud_courant, mot);

            // Passe au nouveau nœud ajouté (le dernier de la liste)
            mon_noeud_courant = (struct Noeud *)lireElement_TabD(mon_noeud_courant->fils, mon_noeud_courant->fils->taille - 1);
        }
    }

    // Retourne le dernier nœud du chemin N-gramme
    return mon_noeud_courant;
}

// Ajout d'un mot au tableau des fils d'un nœud
void ajouterMot_AP(struct Noeud *mon_noeud, char *mot) {
    // Crée un nouveau nœud pour le mot
    struct Noeud *nouveau_noeud = creerNoeud_AP(mot);
    if (nouveau_noeud == NULL) {
        perror("Erreur : nouveau_noeud est NULL - ajouterMot_AP().\n");
        return;
    }

    // Ajoute le nœud dans le tableau dynamique des fils
    void *noeud_generique = (void *)nouveau_noeud;
    ajoutenDernier_TabD(mon_noeud->fils, noeud_generique);
}

// Comparaison entre le mot d'un nœud et un mot donné
int comparerMot_AP(const void *noeud_courant_void, const void *mot) {
    // Cast des pointeurs génériques pour effectuer la comparaison
    const struct Noeud *noeud_courant = (const struct Noeud *)noeud_courant_void;
    const char *motCherche = (const char *)mot;

    // Compare les chaînes de caractères des mots
    return noeud_courant->mot == motCherche;
}

// Affichage du mot d'un nœud
void afficherMot_AP(const void *mon_noeud) {
    // Cast du pointeur générique en pointeur de nœud
    const struct Noeud *mon_noeud_cast = (const struct Noeud *)mon_noeud;

    // Affiche le mot ou un message si le nœud est NULL
    if (mon_noeud_cast != NULL) {
        printf("%s\n", mon_noeud_cast->mot);
    } else {
        printf("Le noeud est NULL.\n");
    }
}

// Recherche ou ajoute un mot parmi les fils d'un nœud final
void rechercherMot_AP(struct Noeud *noeud_final, char *mot) {
    if (noeud_final == NULL) {
        perror("Erreur : noeud_final est NULL - rechercher_ou_ajouter_mot_feuille().\n");
        return;
    }

    if (mot == NULL) {
        perror("Erreur : mot est NULL - rechercher_ou_ajouter_mot_feuille().\n");
        return;
    }

    // Recherche le mot parmi les fils du nœud final
    int index = rechercheElement_TabD(comparerMot_AP, mot, noeud_final->fils);

    if (index != -1) {
        // Si le mot existe, incrémente le nombre d'occurrences
        struct Noeud *noeud_existant = (struct Noeud *)lireElement_TabD(noeud_final->fils, index);
        noeud_existant->occurrences++;
    } else {
        // Sinon, ajoute un nouveau nœud pour le mot
        ajouterMot_AP(noeud_final, mot);
    }
}

// Fonction pour trouver le mot avec le plus grand nombre d'occurence
struct Noeud *MotMaxOccurrences_AP(struct Noeud *noeud_courant) {
    if (noeud_courant == NULL) {
        return NULL; // Si le nœud courant est NULL, on retourne NULL
    }

    // Si le nœud n'a pas de fils, il est considéré comme une feuille, on le retourne
    if (noeud_courant->fils == NULL || noeud_courant->fils->taille == 0) {
        return noeud_courant;
    }

    // Initialiser le nœud ayant le maximum d'occurrences avec le premier fils
    struct Noeud *plus_grand_fils = (struct Noeud *)lireElement_TabD(noeud_courant->fils, 0);
    if (plus_grand_fils == NULL) {
        perror("Erreur : plus_grand_fils est NULL - MotMaxOccurrences_AP().\n");
        return NULL; // Si le nœud courant est NULL, on retourne NULL
    }
    int max_occurrences = plus_grand_fils->occurrences;

    // Parcourir tous les fils pour trouver le nœud avec le maximum d'occurrences
    for (int i = 1; i < noeud_courant->fils->taille; i++) {
        struct Noeud *prochain_fils = (struct Noeud *)lireElement_TabD(noeud_courant->fils, i);
    if (prochain_fils == NULL) {
        perror("Erreur : prochain_fils est NULL - MotMaxOccurrences_AP().\n");
        return NULL; // Si le nœud courant est NULL, on retourne NULL
    }
        // Mettre à jour le maximum si le prochain fils a plus d'occurrences
        if (prochain_fils->occurrences > max_occurrences) {
            max_occurrences = prochain_fils->occurrences;
            plus_grand_fils = prochain_fils;
        }
    }

    // Retourner le nœud ayant le maximum d'occurrences
    return plus_grand_fils;
}



void detruire_arbre(struct Noeud *noeud) {
    if (noeud == NULL) {
        return;  // Si le nœud est NULL, rien à faire
    }

    // Libérer les fils du nœud
    if (noeud->fils != NULL) {
        // Parcourir tous les fils et les détruire récursivement
        for (int i = 0; i < noeud->fils->taille; i++) {
            struct Noeud *fils = (struct Noeud *)lireElement_TabD(noeud->fils, i);
            detruire_arbre(fils);  // Appel récursif pour chaque fils
        }
        // Libérer le tableau dynamique des fils
        detruire_TabD(noeud->fils);
    }

    noeud->mot = NULL;
    free(noeud);  // Libérer la mémoire du nœud lui-même
}



// Fonction récursive pour afficher l'arbre
void afficherArbre(const struct Noeud *racine, int hauteur) {
    if (racine == NULL) {
        return;  // Si le nœud est NULL, on ne fait rien
    }

    // Affichage de l'indentation pour indiquer la hauteur dans l'arbre
    for (int i = 0; i < hauteur; i++) {
        printf("/");
    }

    // Affichage du mot du nœud actuel
    afficherMot_AP(racine);

    // Parcours récursif des fils (si le nœud a des fils)
    for (int i = 0; i < racine->fils->taille; i++) {
        struct Noeud *noeud_fils = (struct Noeud *)lireElement_TabD(racine->fils, i);  // Récupère chaque fils
        afficherArbre(noeud_fils, hauteur + 1);  // Appel récursif pour afficher les sous-nœuds avec une hauteur supérieur
    }
}
