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

// Initialise une sequence avec des mots vides
void sequence_initialise(Sequence *seq, int n, struct strhash_table *ht)
{
    // Vérifier les pointeurs d'entrée
    if (seq == NULL || ht == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Pointeur seq ou ht est NULL dans sequence_initialise");
        return;
    }
    // Ajout du mot vide dans la table de hachage
    char *adresse = strhash_wordAdd(ht, "");
    if (adresse == NULL)
    {
        error_print(ERR_HASH, "SEQUENCE", "Echec de strhash_wordAdd pour le mot vide");
        return;
    }
    // Configuration de la structure
    seq->taille_max = n + 1;
    seq->position = 0;
    // Allocation avec gestion d'erreur
    seq->mots = malloc(seq->taille_max * sizeof(char *));
    if (seq->mots == NULL)
    {
        ERROR_DEBUG(ERR_ALLOC, "Allocation du tableau de mots de la sequence echouee");
        return;
    }
    //  Initialisation du contenu
    for (int i = 0; i < seq->taille_max; i++)
    {
        seq->mots[i] = adresse;
        assert(seq->mots[i] != NULL);
    }
}

// Initialise l'itérateur pour parcourir la sequence
void sequence_itStart(Sequence *seq)
{
    seq->iterateur = (seq->position + 1) % (seq->taille_max); // Positionne l'itérateur après la position courante
    assert(seq->iterateur < seq->taille_max);               // Vérifie que l'itérateur reste dans les limites
}

// Retourne le mot courant pointé par l'itérateur, puis avance l'itérateur
const char *sequence_itNext(Sequence *seq)
{
    // Vérifier que la séquence existe
    if (seq == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tentative de lecture sur une Sequence NULL");
        return ""; // On retourne une chaîne vide pour éviter un crash plus loin
    }
    // Récupération du mot
    char *mot = seq->mots[seq->iterateur];
    // Avancée de l'itérateur
    seq->iterateur = (seq->iterateur + 1) % (seq->taille_max);
    // Gestion d'erreur
    if (mot == NULL)
    {
        ERROR_DEBUG(ERR_INTERNAL, "Un pointeur NULL a ete trouve dans le tableau de mots");
        return "";
    }
    return mot;
}

// Vérifie si l'itérateur a atteint la fin du tableau N_gramme
int sequence_itHasNext(Sequence *seq)
{
    return (seq->iterateur != seq->position); // Retourne 0 si l'itérateur est revenu à la position initiale
}

// Ajoute un nouveau mot à la sequence à la position courante
void sequence_pushWord(Sequence *seq, const char *wordi, struct strhash_table *ht)
{
    seq->mots[seq->position] = strhash_wordAdd(ht, wordi);   // Ajoute le mot à la table de hachage
    seq->position = (seq->position + 1) % (seq->taille_max); // Avance circulairement
    assert(seq->position < seq->taille_max);                 // Vérifie la validité de la position
}

// Retourne le mot à la position courante de la sequence
const char *sequence_nextWord(Sequence *seq)
{
    return seq->mots[seq->position];
}

void sequence_detruire(Sequence *seq)
{
    if (seq != NULL)
    {
        free(seq->mots);
        seq->mots = NULL;
    }
}

// Affiche le contenu actuel de la sequence, séparé par des "/"
void sequence_print(Sequence *seq)
{
    sequence_itStart(seq);
    int premier_mot_affiche = 0; // Pour savoir si on doit mettre un "/"

    while (sequence_itHasNext(seq))
    {
        const char *mot = sequence_itNext(seq);

        // On n'affiche que si le mot n'est pas la chaîne vide d'initialisation
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

// Construit une chaîne de caractères contenant les mots de la sequence
char *sequence_printInTab(Sequence *seq)
{
    static char sequence[256] = ""; // Tableau statique pour stocker la chaîne
    sequence[0] = '\0';             // Réinitialise la chaîne

    sequence_itStart(seq); // Initialise l'itérateur

    // Concatène chaque mot dans la chaîne avec un espace
    while (sequence_itHasNext(seq))
    {
        strcat(sequence, sequence_itNext(seq));
        strcat(sequence, " ");
    }
    return sequence; // Retourne la chaîne construite
}
