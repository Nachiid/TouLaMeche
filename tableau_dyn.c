/************************************************************************
  Nom du fichier : tableau_dyn.c
  Description : contient les fonctions du module tableau dynamique
  Auteur : Nachid Ayman
************************************************************************/

// #define NDEBUG
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tableau_dyn.h"

// Fonction pour initialiser les colonnes d'un tableau dynamique
// Cette fonction remplit la zone mémoire du tableau avec des pointeurs NULL
// à partir d'une position donnée jusqu'à la taille maximale du tableau.
void initialiserColonnes_Tadb(struct table_D *t, int position)
{
    assert(t != NULL);        // Vérifie que la structure de tableau n'est pas NULL
    assert(t->zone != NULL);  // Vérifie que la zone mémoire du tableau est allouée

    // Initialisation de la zone mémoire pour chaque position
    while (position < t->taille_max)
    {
        t->zone[position] = NULL;  // Affecte NULL à chaque élément du tableau
        position++;
    }
}

// Fonction pour créer un tableau dynamique avec une taille maximale et un facteur de croissance
// Cette fonction alloue la mémoire pour la structure du tableau dynamique, initialise
// ses paramètres, et alloue également de la mémoire pour la zone de stockage des éléments.
struct table_D *creer_TabD(int taille_max, int facteur)
{
    // Allocation de la structure de tableau dynamique
    struct table_D *t = malloc(sizeof(struct table_D));
    if (t == NULL)
    {
        perror("Erreur d'allocation pour la structure");
        return NULL;
    }

    // Vérification du facteur de croissance
    if (facteur == 0)
    {
        perror("Erreur d'allocation pour la structure --- facteur = 0 ");
        return NULL;
    }

    // Initialisation des champs de la structure
    t->taille = 0;           // Le tableau est vide au départ
    t->taille_max = taille_max; // Taille maximale du tableau définie par l'utilisateur
    t->facteur = facteur;    // Facteur de croissance pour redimensionner le tableau

    // Allocation de la mémoire pour la zone de stockage du tableau
    t->zone = malloc(taille_max * sizeof(void *));
    if (t->zone == NULL)
    {
        perror("Erreur d'allocation pour la zone");
        free(t);
        return NULL;
    }

    // Initialisation des cases du tableau à NULL
    initialiserColonnes_Tadb(t, 0);

    return t;
}

// Fonction pour détruire un tableau dynamique et libérer la mémoire allouée
// Cette fonction libère la mémoire allouée pour la zone de stockage du tableau
// ainsi que pour la structure du tableau lui-même.
void detruire_TabD(struct table_D *t)
{
    if (t == NULL || t->zone == NULL)
    {
        perror("Erreur : Tableau non détruit.\n");
        return;
    }

    // Libération de la mémoire allouée pour la zone de stockage
    free(t->zone);

    // Réinitialisation des champs de la structure
    t->zone = NULL;
    t->taille = 0;
    t->taille_max = 0;
    t->facteur = 0;

    // Libération de la structure du tableau dynamique
    free(t);
}

// Fonction pour ajuster la taille du tableau dynamique (réallocation)
// Cette fonction augmente la taille maximale du tableau en fonction du facteur de croissance
// jusqu'à ce qu'elle atteigne la taille minimale spécifiée.
void majtaille_TabD(struct table_D *t, int min)
{
    if (t == NULL || t->zone == NULL)
    {
        perror("Erreur : Tableau ou zone null (maj).\n");
        return;
    }

    // Agrandissement de la taille maximale tant qu'elle est inférieure à la taille minimale requise
    while (t->taille_max < min)
    {
        t->taille_max = t->taille_max * t->facteur;  // Augmentation de la taille du tableau selon le facteur
    }

    // Réallocation de la zone mémoire avec la nouvelle taille
    void **zone = realloc(t->zone, t->taille_max * sizeof(void *));
    if (zone == NULL)
    {
        perror("Erreur : réallocation mémoire");
        free(t->zone);  // Libération de la mémoire précédente en cas d'échec
        return;
    }

    t->zone = zone;

    // Initialisation des nouvelles cases allouées à NULL
    initialiserColonnes_Tadb(t, t->taille);
}

// Fonction pour ajouter un élément à une position spécifique dans le tableau dynamique
// Cette fonction insère l'élément à la position demandée, en redimensionnant si nécessaire
// et en décalant les éléments suivants pour faire de la place.
void ajoutElement_TabD(struct table_D *t, void *element, int position)
{
    // Vérification de la position
    if (position < 0)
    {
        perror("Erreur : ajoutElement_TabD -- Position negative");
        return;
    }

    // Si le tableau est vide, initialisation de l'élément à la position donnée
    if (t->taille == 0)
    {
        if (position > t->taille_max - 1)
        {
            majtaille_TabD(t, position);  // Agrandissement de la taille si nécessaire
            t->zone[position] = element;
            t->taille = position + 1;
        }
        else
        {
            t->zone[position] = element;
            t->taille++;
        }
    }
    else if (position > t->taille - 1)  // Si l'élément doit être ajouté à une position au-delà de la taille actuelle
    {
        if (position > t->taille_max - 1)
        {
            // Si nécessaire, on agrandit le tableau
            majtaille_TabD(t, position);
        }
        t->taille = position + 1;
        t->zone[position] = element;
    }
    else if (position <= t->taille - 1)  // Si la position est valide dans la taille actuelle
    {
        if (t->taille == t->taille_max)
        {
            // Si le tableau est plein, on agrandit sa taille
            majtaille_TabD(t, t->taille_max + 1);
        }

        if (t->zone[position] == NULL)
        {
            t->zone[position] = element;
        }
        else
        {
            // Décalage des éléments suivants pour insérer l'élément à la position désirée
            for (int i = t->taille; i > position; i--)
            {
                t->zone[i] = t->zone[i - 1];
            }
            t->zone[position] = element;
            t->taille++;
        }
    }
}

// Fonction pour ajouter un élément à la dernière position du tableau
// Cette fonction appelle la fonction `ajoutElement_TabD` en spécifiant la dernière position.
void ajoutenDernier_TabD(struct table_D *t, void *element)
{
    ajoutElement_TabD(t, element, t->taille);
}

// Fonction pour afficher un élément à une position donnée dans le tableau
// Cette fonction appelle une fonction de rappel pour afficher l'élément sous une forme générique.
// Si l'élément est NULL, affiche "NULL".
void AfficherElement_Tabd(void (*pf)(const void *), struct table_D *t, int position)
{
    if (position < 0 || position >= t->taille)
    {
        perror("Erreur : position hors limites AfficherElement_Tabd() \n");
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

// Fonction pour lire un élément à une position donnée dans le tableau
// Cette fonction retourne l'élément stocké à la position spécifiée, ou NULL si la position est invalide.
void *lireElement_TabD(struct table_D *t, int position)
{
    if (position < 0 || position >= t->taille)
    {
        perror("Erreur : position hors limites lireElement_TabD() \n");
        return NULL;
    }
    return t->zone[position];
}

// Fonction pour écrire un élément à une position donnée dans le tableau
// Si la position est valide, l'élément est écrit à cette position.
// Si la position est au-delà de la taille actuelle, l'élément est ajouté.
void ecrireElement_TabD(struct table_D *t, void *element, int position)
{
    if (position < 0)
    {
        fprintf(stderr, "Erreur : position hors limites -- ecrireElement_TabD\n");
        return;
    }

    // Si la position est au-delà de la taille actuelle, on ajoute l'élément
    if (position > t->taille)
    {
        ajoutElement_TabD(t, element, position);
    }
    else
    {
        // Sinon, on remplace l'élément existant
        t->zone[position] = element;
    }
}

// Fonction pour rechercher un élément dans le tableau dynamique
// Cette fonction parcourt le tableau et retourne l'index de l'élément trouvé
// en utilisant une fonction de comparaison. Si l'élément n'est pas trouvé, elle retourne -1.
int rechercheElement_TabD(int (*pf)(const void *, const void *), void *element, struct table_D *t)
{
    // Parcours de chaque élément du tableau pour trouver la correspondance
    for (int i = 0; i < t->taille; i++)
    {
        if (t->zone[i] != NULL)
        {
            if (pf(t->zone[i], element))
            {
                return i;  // Retourne l'index de l'élément trouvé
            }
        }
    }
    return -1;  // Si l'élément n'est pas trouvé, retourne -1
}
