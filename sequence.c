/************************************************************************
  Nom du fichier : sequence.c
=============================================================
  Description : Contient les fonctions du module sequence
=============================================================
  Auteur : Nachid Ayman
=============================================================
************************************************************************/

// #define NDEBUG  // Décommenter pour désactiver les assertions

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "hash.h"
#include "list.h"
#include "sequence.h"
#include "test.h"

// Alloue et initialise une séquence de taille n+1
Sequence *sequence_creer(int n, struct strhash_table *ht)
{
    if (ht == NULL)
        return NULL;

    // Allocation de la structure
    Sequence *seq = malloc(sizeof(Sequence));
    if (seq == NULL)
    {
        error_print(ERR_ALLOC, "SEQUENCE", "Echec malloc structure Sequence");
        return NULL;
    }

    // Ajout du mot vide dans la table de hachage
    char *adresse_vide = strhash_wordAdd(ht, "");

    // Initialisation des champs
    seq->taille_max = n + 1;
    seq->position = 0;
    seq->iterateur = 0;

    // Allocation du tableau de mots
    seq->mots = malloc(seq->taille_max * sizeof(char *));
    if (seq->mots == NULL)
    {
        free(seq);
        return NULL;
    }

    // Remplissage initial avec le mot vide
    for (int i = 0; i < seq->taille_max; i++)
    {
        seq->mots[i] = adresse_vide;
    }

    return seq;
}

// Positionne l'itérateur juste après la position courante
void sequence_itStart(Sequence *seq)
{
    seq->iterateur = (seq->position + 1) % (seq->taille_max);
    assert(seq->iterateur < seq->taille_max);
}

// Retourne le mot courant et avance l'itérateur
const char *sequence_itNext(Sequence *seq)
{
    if (seq == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tentative de lecture sur une Sequence NULL");
        return "";
    }

    char *mot = seq->mots[seq->iterateur];
    seq->iterateur = (seq->iterateur + 1) % (seq->taille_max);

    if (mot == NULL)
    {
        ERROR_DEBUG(ERR_INTERNAL, "Un pointeur NULL a ete trouve dans le tableau de mots");
        return "";
    }

    return mot;
}

// Retourne 1 si l'itérateur n'a pas encore atteint la position courante
int sequence_itHasNext(Sequence *seq)
{
    return (seq->iterateur != seq->position);
}

// Insère un mot à la position courante et avance circulairement
void sequence_pushWord(Sequence *seq, const char *wordi, struct strhash_table *ht)
{
    seq->mots[seq->position] = strhash_wordAdd(ht, wordi);
    seq->position = (seq->position + 1) % (seq->taille_max);
    assert(seq->position < seq->taille_max);
}

// Retourne le mot à la position courante
const char *sequence_nextWord(Sequence *seq)
{
    return seq->mots[seq->position];
}

// Libère la mémoire de la séquence
void sequence_detruire(Sequence *seq)
{
    if (seq == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "sequence_detruire called with NULL pointer");
        return;
    }

    if (seq->mots == NULL)
    {
        ERROR_DEBUG(ERR_SEQUENCE, "sequence->mots is NULL, skipping free");
    }
    else
    {
        free(seq->mots);
        seq->mots = NULL;
    }

    // Réinitialisation des champs avant libération
    seq->taille_max = 0;
    seq->position = 0;
    seq->iterateur = 0;

    free(seq);

    return;
}

// Affiche les mots non vides de la séquence séparés par " / "
void sequence_print(Sequence *seq)
{
    sequence_itStart(seq);
    int premier_mot_affiche = 0;

    while (sequence_itHasNext(seq))
    {
        const char *mot = sequence_itNext(seq);

        // Ignore les mots vides d'initialisation
        if (mot[0] != '\0')
        {
            if (premier_mot_affiche)
            {
                printf(" / ");
            }
            printf("%s", mot);
            premier_mot_affiche = 1;
        }
    }
    printf("\n");
}

// Retourne les mots de la séquence concaténés dans un buffer statique
char *sequence_printInTab(Sequence *seq)
{
    static char sequence[256] = "";
    sequence[0] = '\0';

    sequence_itStart(seq);

    // Concatène chaque mot suivi d'un espace
    while (sequence_itHasNext(seq))
    {
        strcat(sequence, sequence_itNext(seq));
        strcat(sequence, " ");
    }

    return sequence;
}