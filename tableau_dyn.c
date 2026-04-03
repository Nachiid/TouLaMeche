/************************************************************************
  Nom du fichier : tableau_dyn.c
=============================================================
  Description : Contient les fonctions du module tableau dynamique
=============================================================
  Auteur : Nachid Ayman
=============================================================
************************************************************************/

// #define NDEBUG
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "tableau_dyn.h"

// Initialise à NULL les cases du tableau à partir d'une position donnée
void initialiserColonnes_Tadb(struct table_D *t, int position)
{
    if (t == NULL || t->zone == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tentative d'initialisation sur un tableau ou une zone NULL");
        return;
    }

    while (position < t->taille_max)
    {
        t->zone[position] = NULL;
        position++;
    }
}

// Alloue et initialise un tableau dynamique avec une taille et un facteur de croissance
struct table_D *creer_TabD(int taille_max, int facteur)
{
    // Vérification des arguments
    if (facteur <= 0)
    {
        error_print(ERR_INVALID_ARGUMENT, "DYNARRAY", "Le facteur de croissance doit etre > 0");
        return NULL;
    }
    if (taille_max < 1)
    {
        taille_max = 1;
    }

    // Allocation de la structure
    struct table_D *t = malloc(sizeof(struct table_D));
    if (t == NULL)
    {
        error_print(ERR_ALLOC, "DYNARRAY", "Echec malloc structure table_D");
        return NULL;
    }

    // Initialisation des champs
    t->taille = 0;
    t->taille_max = taille_max;
    t->facteur = facteur;

    // Allocation de la zone de stockage
    t->zone = malloc(taille_max * sizeof(void *));
    if (t->zone == NULL)
    {
        error_print(ERR_ALLOC, "DYNARRAY", "Echec malloc zone de stockage");
        free(t);
        return NULL;
    }

    initialiserColonnes_Tadb(t, 0);

    return t;
}

// Libère la mémoire du tableau dynamique
void detruire_TabD(struct table_D *t)
{
    if (t == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tentative de destruction d'un pointeur NULL");
        return;
    }

    if (t->zone != NULL)
    {
        free(t->zone);
    }

    // Réinitialisation des champs avant libération
    t->zone = NULL;
    t->taille = 0;
    t->taille_max = 0;
    t->facteur = 0;

    free(t);
}

// Agrandit la zone mémoire jusqu'à atteindre la taille minimale requise
void majtaille_TabD(struct table_D *t, int min)
{
    if (t == NULL || t->zone == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tableau ou zone null (maj)");
        return;
    }

    // Multiplie la taille par le facteur jusqu'à atteindre min
    while (t->taille_max < min)
    {
        t->taille_max = t->taille_max * t->facteur;
    }

    // Réallocation avec la nouvelle taille
    void **nouvelle_zone = realloc(t->zone, t->taille_max * sizeof(void *));
    if (nouvelle_zone == NULL)
    {
        error_print(ERR_ALLOC, "DYNARRAY", "Echec de realloc de la zone memoire");
        // t->zone est conservé pour ne pas perdre les données existantes
        return;
    }

    t->zone = nouvelle_zone;

    // Initialisation des nouvelles cases à NULL
    initialiserColonnes_Tadb(t, t->taille);
}

// Insère un élément à la position donnée, en décalant les suivants si nécessaire
void ajoutElement_TabD(struct table_D *t, void *element, int position)
{
    if (position < 0)
    {
        error_print(ERR_INVALID_ARGUMENT, "DYNARRAY", "Position negative interdite");
        return;
    }

    // Agrandissement si la position dépasse la capacité physique
    if (position >= t->taille_max)
    {
        majtaille_TabD(t, position + 1);
    }

    // Décalage vers la droite si la case est déjà occupée
    if (position < t->taille && t->zone[position] != NULL)
    {
        // Agrandissement si le tableau est plein
        if (t->taille == t->taille_max)
        {
            majtaille_TabD(t, t->taille_max + 1);
        }

        int nb_elements_a_deplacer = t->taille - position;

        memmove(&t->zone[position + 1],
                &t->zone[position],
                nb_elements_a_deplacer * sizeof(void *));
        t->taille++;
    }

    // Insertion de l'élément
    t->zone[position] = element;

    // Mise à jour de la taille logique si on écrit au-delà
    if (position >= t->taille)
    {
        t->taille = position + 1;
    }
}

// Ajoute un élément en dernière position
void ajoutenDernier_TabD(struct table_D *t, void *element)
{
    ajoutElement_TabD(t, element, t->taille);
}

// Affiche l'élément à la position donnée via une fonction de rappel
void AfficherElement_Tabd(void (*pf)(const void *), const struct table_D *t, int position)
{
    if (t == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tableau NULL dans AfficherElement_Tabd");
        return;
    }

    if (position < 0 || position >= t->taille)
    {
        error_print(ERR_INVALID_ARGUMENT, "DYNARRAY", "Position hors limites dans AfficherElement_Tabd");
        return;
    }

    if (t->zone[position] == NULL)
    {
        printf("NULL\n");
    }
    else
    {
        pf(t->zone[position]);
    }
}

// Retourne l'élément à la position donnée, ou NULL si invalide
void *lireElement_TabD(const struct table_D *t, int position)
{
    if (t == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tableau NULL dans lireElement_TabD");
        return NULL;
    }

    if (position < 0 || position >= t->taille)
    {
        error_print(ERR_INVALID_ARGUMENT, "DYNARRAY", "Position hors limites dans lireElement_TabD");
        return NULL;
    }

    return t->zone[position];
}

// Écrit un élément à la position donnée, ou l'ajoute si hors de la taille actuelle
void ecrireElement_TabD(struct table_D *t, void *element, int position)
{
    if (t == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tableau NULL dans ecrireElement_TabD");
        return;
    }

    if (position < 0)
    {
        error_print(ERR_INVALID_ARGUMENT, "DYNARRAY", "Position negative dans ecrireElement_TabD");
        return;
    }

    if (position >= t->taille)
    {
        ajoutElement_TabD(t, element, position);
    }
    else
    {
        // Remplacement de l'élément existant
        t->zone[position] = element;
    }
}

// Retourne l'index du premier élément correspondant, ou -1 si non trouvé
int rechercheElement_TabD(int (*pf)(const void *, const void *), const void *element, const struct table_D *t)
{
    if (t == NULL || pf == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Argument NULL dans rechercheElement_TabD");
        return -1;
    }

    for (int i = 0; i < t->taille; i++)
    {
        if (t->zone[i] != NULL)
        {
            if (pf(t->zone[i], element))
            {
                return i;
            }
        }
    }

    return -1;
}