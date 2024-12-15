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
#include "hash.h"
#include "list.h"
#include "sequence.h"
#include "test.h"

// Tableau de séquence N-gramme de taille Lg_N_gramme + 1
static char *N_gramme[Lg_N_gramme + 1]; 

// Itérateur et position courante dans le tableau N_gramme
static int iterateur, position;

// Initialise le tableau N_gramme avec des mots vides
void sequence_initialise(struct strhash_table *ht)
{
    // Ajoute une chaîne vide à la table de hachage et récupère son adresse
    char *adresse = strhash_wordAdd(ht, "");
    if (adresse == NULL)
    {
        perror("Erreur : création de la table strhash_wordAdd() échouée");
    }

    // Initialise chaque entrée de N_gramme avec cette adresse
    for (int i = 0; i < Lg_N_gramme + 1; i++)
    {
        N_gramme[i] = adresse;
        assert(N_gramme[i] != NULL); // Vérifie que l'initialisation a réussi
    }

    position = 0; // Position initiale dans le tableau
}

// Initialise l'itérateur pour parcourir le tableau N_gramme
void sequence_itStart(void)
{
    iterateur = (position + 1) % (Lg_N_gramme + 1); // Positionne l'itérateur après la position courante
    assert(iterateur < Lg_N_gramme + 1); // Vérifie que l'itérateur reste dans les limites
}

// Retourne le mot courant pointé par l'itérateur, puis avance l'itérateur
const char *sequence_itNext(void)
{
    char *mot = N_gramme[iterateur];
    iterateur = (iterateur + 1) % (Lg_N_gramme + 1); // Avance à la position suivante de manière circulaire

    if (mot == NULL)
    {
        perror("Erreur : affectation d'une valeur NULL au mot");
    }
    return mot;
}

// Vérifie si l'itérateur a atteint la fin du tableau N_gramme
int sequence_itHasNext(void)
{
    return (iterateur != position); // Retourne 0 si l'itérateur est revenu à la position initiale
}

// Ajoute un nouveau mot au tableau N_gramme à la position courante
void sequence_addWord(const char *wordi, struct strhash_table *ht)
{
    N_gramme[position] = strhash_wordAdd(ht, wordi); // Ajoute le mot à la table de hachage
}

// Retourne le mot à la position courante du tableau N_gramme
const char *sequence_nextWord(void)
{
    return N_gramme[position];
}

// Fait avancer la position dans le tableau N_gramme pour intégrer un nouveau mot
void sequence_progress(void)
{
    position = (position + 1) % (Lg_N_gramme + 1); // Avance circulairement
    assert(position < Lg_N_gramme + 1); // Vérifie la validité de la position
}

// Affiche le contenu actuel du tableau N_gramme, séparé par des "/"
void sequence_print(void)
{
    sequence_itStart(); // Initialise l'itérateur
    const char *mot = sequence_itNext(); // Récupère le premier mot
    printf("%s", mot);

    // Parcourt et affiche les mots suivants
    while (sequence_itHasNext())
    {
        mot = sequence_itNext();
        printf("/ %s", mot);
    }
    printf("\n"); // Nouvelle ligne après l'affichage
}

// Construit une chaîne de caractères contenant les mots du tableau N_gramme
char *sequence_printInTab(void)
{
    static char sequence[256] = ""; // Tableau statique pour stocker la chaîne
    sequence[0] = '\0'; // Réinitialise la chaîne

    sequence_itStart(); // Initialise l'itérateur

    // Concatène chaque mot dans la chaîne avec un espace
    while (sequence_itHasNext())
    {
        strcat(sequence, sequence_itNext());
        strcat(sequence, " ");
    }
    return sequence; // Retourne la chaîne construite
}
