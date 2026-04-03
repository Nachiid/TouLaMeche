/************************************************************************
  Nom du fichier : sequence.h
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

// Séquence circulaire de mots (N-gramme)
typedef struct
{
  char **mots;    // Tableau de pointeurs vers les mots (stockés dans la table de hachage)
  int taille_max; // Capacité totale du tableau (N + 1)
  int position;   // Indice d'écriture courant
  int iterateur;  // Indice de parcours courant
} Sequence;

/* --- Gestion de la structure --- */

// Alloue et initialise une séquence de taille n+1
Sequence *sequence_creer(int n, struct strhash_table *ht);

// Libère la mémoire de la séquence
void sequence_detruire(Sequence *seq);

/* --- Modification --- */

// Insère un mot à la position courante et avance circulairement
void sequence_pushWord(Sequence *seq, const char *wordi, struct strhash_table *ht);

/* --- Itérateur --- */

// Positionne l'itérateur juste après la position courante
void sequence_itStart(Sequence *seq);

// Retourne 1 si l'itérateur n'a pas encore atteint la position courante
int sequence_itHasNext(Sequence *seq);

// Retourne le mot courant et avance l'itérateur
const char *sequence_itNext(Sequence *seq);

// Retourne le mot à la position courante
const char *sequence_nextWord(Sequence *seq);

/* --- Affichage --- */

// Affiche les mots non vides de la séquence séparés par " / "
void sequence_print(Sequence *seq);

// Retourne les mots de la séquence concaténés dans un buffer statique
char *sequence_printInTab(Sequence *seq);

#endif /* _SEQUENCE_H */