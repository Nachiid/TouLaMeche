/************************************************************************
  Nom du fichier : sequence.c
=============================================================
  Description : Contient les fonctions du module sequence
=============================================================
  Auteur : Nachid Ayman
=============================================================
************************************************************************/

#ifndef _SEQUENCE_H
#define _SEQUENCE_H

#include "list.h"
#include "hash.h"

/**
 * Structure représentant une séquence circulaire de mots (N-gramme).
 * L'encapsulation permet de gérer plusieurs séquences indépendamment.
 */
typedef struct
{
    char **mots;    // Tableau de pointeurs vers les mots stockés dans la table de hachage
    int taille_max; // Capacité totale du tableau (N + 1 pour la gestion du buffer circulaire)
    int position;   // Indice d'écriture actuel
    int iterateur;  // Indice utilisé pour le parcours de la séquence
} Sequence;

/* --- Fonctions de gestion de la structure --- */

// Alloue la mémoire et initialise la séquence avec des chaînes vides
void sequence_initialise(Sequence *seq, int n, struct strhash_table *ht);

// Libère la mémoire allouée pour le tableau de mots
void sequence_detruire(Sequence *seq);

/* --- Fonctions de modification --- */

// Ajoute un mot à la séquence et avance le curseur d'écriture de manière circulaire
void sequence_pushWord(Sequence *seq, const char *wordi, struct strhash_table *ht);

/* --- Fonctions d'accès et de parcours (Itérateur) --- */

// Positionne l'itérateur au début de la séquence logique
void sequence_itStart(Sequence *seq);

// Vérifie s'il reste des mots à parcourir avec l'itérateur
int sequence_itHasNext(Sequence *seq);

// Retourne le mot pointé par l'itérateur et passe au suivant
const char *sequence_itNext(Sequence *seq);

// Retourne le mot situé à la position actuelle du curseur d'écriture
const char *sequence_nextWord(Sequence *seq);

/* --- Fonctions d'affichage et de formatage --- */

// Affiche la séquence sur la sortie standard (format: mot1 / mot2 / ...)
void sequence_print(Sequence *seq);

// Retourne une représentation textuelle de la séquence sous forme de chaîne
char *sequence_printInTab(Sequence *seq);

#endif /* _SEQUENCE_H */