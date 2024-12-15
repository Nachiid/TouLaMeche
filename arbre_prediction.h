/************************************************************************
  Nom du fichier : arbre_prediction.h
  Description : Déclaration du module pour l'arbre de prédiction.
  Auteur : Nachid Ayman
************************************************************************/

#ifndef ARBRE_PREDICTION_H
#define ARBRE_PREDICTION_H

// Inclusion de fichiers nécessaires
#include "list.h"             // Inclusion d'un fichier de gestion de listes (probablement une structure de liste chaînée)
#include "tableau_dyn.h"       // Inclusion d'un fichier pour la gestion des tableaux dynamiques

// Définition de la structure représentant un nœud de l'arbre de prédiction
struct Noeud {
    char *mot;               // Le mot associé au nœud, représenté par une chaîne de caractères
    int occurrences;         // Le nombre d'occurrences du mot dans l'arbre
    struct table_D *fils;    // Tableau dynamique contenant les fils du nœud, chaque fils est un autre nœud
};

// Déclaration des fonctions disponibles dans le module

// Crée un nœud avec un mot donné et initialise ses attributs
struct Noeud *creerNoeud_AP(char *mot);

// Crée la racine de l'arbre avec un mot vide
struct Noeud *creerRacine_AP();

// Recherche ou complète un N-gramme dans l'arbre à partir d'un nœud racine donné
struct Noeud *rechercher_completer_ngramme_AP(struct Noeud *racine);

// Ajoute un mot à la liste des fils d'un nœud donné
void ajouterMot_AP(struct Noeud *mon_noeud, char* mot);

// Fonction de comparaison entre un mot donné et un mot d'un nœud
int comparerMot_AP(const void *noeud_courant_void, const void *mot);

// Affiche le mot d'un nœud
void afficherMot_AP(const void *mon_noeud);

// Recherche un mot parmi les fils d'un nœud final et l'ajoute si nécessaire
void rechercherMot_AP(struct Noeud *noeud_final, char *mot);

// Recherche le nœud ayant le plus grand nombre d'occurrences parmi les fils d'un nœud donné
struct Noeud *MotMaxOccurrences_AP(struct Noeud *noeud_courant);

// Libere la mémoire de l'arbre de prédiction
void detruire_arbre(struct Noeud *noeud);

// Fonction qui affiche une arbre de prediction
void afficherArbre(const struct Noeud *racine, int niveau);

#endif // ARBRE_PREDICTION_H
