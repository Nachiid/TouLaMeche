#ifndef _SEQUENCE_H
#define _SEQUENCE_H

// Inclusion des fichiers nécessaires pour le module sequence
#include "list.h"
#include "hash.h"
#include "test.h"

// Définition de la taille du N-gramme
enum
{
    Lg_N_gramme = 3  // Taille du N-gramme
};

// Déclaration des fonctions du module sequence
void sequence_initialise(struct strhash_table *ht); // Initialise le N-gramme avec des mots vides
void sequence_itStart(void); // Initialise l'itérateur pour parcourir le N-gramme
const char *sequence_itNext(void); // Retourne le mot suivant et fait avancer l'itérateur
int sequence_itHasNext(void); // Vérifie si l'itérateur a encore des éléments à parcourir
void sequence_addWord(const char *wordi, struct strhash_table *ht); // Ajoute un mot au N-gramme
const char *sequence_nextWord(void); // Retourne le mot à la position actuelle du N-gramme
void sequence_progress(void); // Fait avancer le N-gramme pour intégrer un nouveau mot
void sequence_print(void); // Affiche le contenu du N-gramme
char *sequence_printInTab(void); // Retourne une chaîne de caractères représentant le N-gramme

#endif /* _SEQUENCE_H */
