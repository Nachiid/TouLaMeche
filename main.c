/************************************************************************
  Nom du fichier : main.c
  Description : Le main
  Auteur : Nachid Ayman
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hash.h"          // Inclusion du fichier de la table de hachage
#include "list.h"          // Inclusion du fichier de la liste
#include "sequence.h"      // Inclusion du fichier de la séquence de mots
#include "tableau_dyn.h"   // Inclusion du fichier de gestion du tableau dynamique
#include "test.h"          // Inclusion du fichier de tests
#include "arbre_prediction.h"

int main()
{
    // Tester les fonctions de sequence
   /* if (test_sequence())
    {
        fprintf(stderr,"Test non reussi\n");
        return 1;  // Si le test de la séquence échoue, quitter le programme avec une erreur
    }*/
    // Tester les fonctions du tableau dynamique
    if (test_TabD())
    {
        fprintf(stderr,"Test Tableau dynamique non reussi\n");
        return 1;  // Si le test du tableau dynamique échoue, quitter avec une erreur
    }

    if (test_AP())
    {
        fprintf(stderr,"Test Arbre de prédiction non reussi\n");
        return 1;  // Si le test du Arbre de prédiction échoue, quitter avec une erreur
    }

    // Main avec arbre

    // Création d'une table de hachage de taille définie par la constante ENTREE
    struct strhash_table *ht = strhash_create(ENTREE);
    
    // Initialisation de la séquence avec la table de hachage
    sequence_initialise(ht);
    sequence_itStart();  // Démarre l'itérateur de la séquence

    // Ajout de plusieurs mots à la séquence et progression à chaque étape
    sequence_addWord("Aymen", ht);  // Ajoute "Aymen" à la séquence
    sequence_progress();            // Avance dans la séquence (pour indiquer que l'ajout est fait)
    sequence_addWord("Nachid", ht);  // Ajoute "Nachid" à la séquence
    sequence_progress();             // Avance dans la séquence
    sequence_addWord("Casa", ht);    // Ajoute "Casa" à la séquence
    sequence_progress();             // Avance dans la séquence

    // Création de l'arbre de prédiction (initialisation de la racine)
    struct Noeud *racine = creerRacine_AP();
    if (racine == NULL) {
        fprintf(stderr,"Erreur 1: La racine est NULL.\n");
        return 1;
    }
    if (racine->fils->taille != 0) {
        fprintf(stderr,"Erreur 2: racine->fils->taille est %d au lieu de 0.\n", racine->fils->taille);
        return 1;
    }

    // Affichage de l'arbre après la création de la racine (avant l'ajout de mots)
    printf("\nAffichage de l'arbre après la création de la racine :\n");
    afficherArbre(racine, 0);

    // Ajout de mots au nœud racine (avec test de l'ajout)
    ajouterMot_AP(racine, "mon");
    ajouterMot_AP(racine, "test");
    ajouterMot_AP(racine, "perso");
    ajouterMot_AP(racine, "bon");

    if (racine->fils->taille != 4) {
        fprintf(stderr,"Erreur 3: racine->fils->taille est %d au lieu de 4.\n", racine->fils->taille);
        return 1;
    }

    // Affichage de l'arbre après ajout des mots
    printf("\nAffichage de l'arbre après ajout des mots sous la racine :\n");
    afficherArbre(racine, 0);

    // Recherche de la séquence qui n'existe pas dans l'arbre (Aymen Nachid Casa)
    struct Noeud *resultat_noeud = rechercher_completer_ngramme_AP(racine);
    if (resultat_noeud == NULL) {
        fprintf(stderr,"Erreur 4: Le resultat_noeud est NULL.\n");
    }
    if (resultat_noeud->mot == NULL) {
        fprintf(stderr,"Erreur 5: Le resultat_noeud->mot est NULL.\n");
    }
    if (racine->fils->taille != 5) {
        fprintf(stderr,"Erreur 6: racine->fils->taille est %d au lieu de 5.\n", racine->fils->taille);
        return 1;
    }

    // Affichage de l'arbre après la recherche
    printf("\nAffichage de l'arbre après recherche de la séquence (Aymen Nachid Casa) :\n");
    afficherArbre(racine, 0);

    // Recherche 2 de la séquence existante dans l'arbre
    struct Noeud *noeud_existant = rechercher_completer_ngramme_AP(racine);
    if (noeud_existant == NULL) {
        fprintf(stderr,"Erreur 11: Le noeud_existant est NULL.\n");
    }
    if (noeud_existant->mot == NULL) {
        fprintf(stderr,"Erreur 12: Le noeud_existant->mot est NULL.\n");
    }
    if (strcmp(noeud_existant->mot, "Casa")) {
        fprintf(stderr,"Erreur 13: Le noeud_existant->mot est incorrect.\n");
    }
    if (noeud_existant != resultat_noeud) {
        fprintf(stderr,"Erreur 14: Erreur fonction recherche.\n");
    }

    // Effectuer des recherches supplémentaires pour voir si le nombre d'occurence du mot "test" est ajouté puis incrementé
    rechercherMot_AP(noeud_existant, "test");
    rechercherMot_AP(noeud_existant, "test");
    rechercherMot_AP(noeud_existant, "test");
    rechercherMot_AP(noeud_existant, "test");

    // Recherche du mot "Cool", qui n'est pas dans l'arbre
    rechercherMot_AP(noeud_existant, "Cool");

    // Récupération et affichage du nœud ayant le plus grand nombre d'occurrences parmi les fils
    struct Noeud *resultat_noeud3 = (struct Noeud *)lireElement_TabD(noeud_existant->fils, 0);

    if (resultat_noeud3->occurrences != 4) {
        fprintf(stderr,"Erreur 15: noeud_existant->occurrences, Nombre d'occurence est : %d au lieu de 4.\n", resultat_noeud3->occurrences);
    }
    resultat_noeud3 = (struct Noeud *)lireElement_TabD(noeud_existant->fils, 1); // Cool
    if (resultat_noeud3->occurrences != 1) {
        fprintf(stderr,"Erreur 16: noeud_existant->occurrences, Nombre d'occurence est : %d au lieu de 1.\n", resultat_noeud3->occurrences);
    }

    // Affichage final de l'arbre après toutes les opérations
    printf("\nAffichage final de l'arbre après les recherches et incrémentations des occurrences :\n");
    afficherArbre(racine, 0);

    // Libération de la mémoire allouée pour l'arbre de prédiction
    detruire_arbre(racine);
    strhash_free(ht);  // Libère la mémoire de la table de hachage
    return 0;  // Retourne 0 pour indiquer que le test s'est terminé avec succès
    
}
