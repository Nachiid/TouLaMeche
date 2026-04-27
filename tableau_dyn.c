/************************************************************************
 * File    : tableau_dyn.c
 * =============================================================
 * Description : Implements a generic dynamic array (growable buffer)
 *               storing void* pointers. The array expands automatically
 *               by a configurable growth factor whenever the capacity
 *               is exceeded.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

/* Uncomment to disable all assertions (production builds) */
// #define NDEBUG

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "tableau_dyn.h"

/*
 * initialiserColonnes_Tabd - Zero-initialize slots from @position to end of buffer
 * @t:        Target dynamic array
 * @position: Index of the first slot to initialize
 *
 * Sets every slot from @position up to taille_max - 1 to NULL.
 * Called after allocation or reallocation to ensure no slot holds a
 * stale (uninitialized) pointer.
 */
void initialiserColonnes_Tabd(struct table_D *t, int position)
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

/*
 * creer_TabD - Allocate and initialize a dynamic array
 * @taille_max: Initial buffer capacity (clamped to 1 if < 1)
 * @facteur:    Multiplicative growth factor applied on each resize (must be > 0)
 *
 * Returns: A pointer to the new dynamic array, or NULL on invalid arguments
 *          or allocation failure.
 */
struct table_D *creer_TabD(int taille_max, int facteur)
{
    /* Validate growth factor before touching the heap */
    if (facteur <= 1)
    {
        error_print(ERR_INVALID_ARGUMENT, "DYNARRAY", "Le facteur de croissance doit etre > 0");
        return NULL;
    }
    if (taille_max < 1)
    {
        taille_max = 1;
    }

    /* Allocate the control structure */
    struct table_D *t = malloc(sizeof(struct table_D));
    if (t == NULL)
    {
        error_print(ERR_ALLOC, "DYNARRAY", "Echec malloc structure table_D");
        return NULL;
    }

    /* Initialize metadata fields before allocating storage */
    t->taille = 0;
    t->taille_max = taille_max;
    t->facteur = facteur;

    /* Allocate the pointer storage zone */
    t->zone = malloc(taille_max * sizeof(void *));
    if (t->zone == NULL)
    {
        error_print(ERR_ALLOC, "DYNARRAY", "Echec malloc zone de stockage");
        free(t);
        return NULL;
    }

    initialiserColonnes_Tabd(t, 0);

    return t;
}

/*
 * detruire_TabD - Free all resources held by a dynamic array
 * @t: Array to destroy; NULL is tolerated (logs a debug error)
 *
 * Frees the storage zone and the control structure. Fields are zeroed
 * before freeing to catch dangling-pointer bugs early.
 * Note: elements pointed to by the array are NOT freed here.
 */
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

    /* Zero fields defensively before freeing, to expose use-after-free */
    t->zone = NULL;
    t->taille = 0;
    t->taille_max = 0;
    t->facteur = 0;

    free(t);
}

/*
 * majtaille_TabD - Grow the array's storage until it can hold at least @min elements
 * @t:   Target dynamic array
 * @min: Minimum required capacity after the resize
 *
 * Repeatedly multiplies taille_max by the growth factor until taille_max >= @min,
 * then reallocates the storage zone accordingly. Newly created slots are
 * initialized to NULL. On realloc failure the existing data is preserved.
 */
void majtaille_TabD(struct table_D *t, int min)
{
    if (t == NULL || t->zone == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tableau ou zone null (maj)");
        return;
    }

    /* Double (or multiply) until the capacity satisfies the minimum */
    while (t->taille_max < min)
    {
        t->taille_max = t->taille_max * t->facteur;
    }

    /* Reallocate the storage zone to the new capacity */
    void **nouvelle_zone = realloc(t->zone, t->taille_max * sizeof(void *));
    if (nouvelle_zone == NULL)
    {
        error_print(ERR_ALLOC, "DYNARRAY", "Echec de realloc de la zone memoire");
        /* Keep t->zone intact to avoid losing existing data */
        return;
    }

    t->zone = nouvelle_zone;

    /* Initialize only the newly allocated slots (from old taille onward) */
    initialiserColonnes_Tabd(t, t->taille);
}

/*
 * ajoutElement_TabD - Insert an element at the given position, shifting if needed
 * @t:        Target dynamic array
 * @element:  Pointer to the element to insert
 * @position: Index at which to insert (must be >= 0)
 *
 * If @position exceeds the current capacity, the array is grown first.
 * If the slot at @position is already occupied, existing elements from
 * @position onward are shifted right by one slot before insertion.
 * The logical size (taille) is updated accordingly.
 */
void ajoutElement_TabD(struct table_D *t, void *element, int position)
{
    if (position < 0)
    {
        error_print(ERR_INVALID_ARGUMENT, "DYNARRAY", "Position negative interdite");
        return;
    }

    /* Grow the buffer if the target position is beyond current capacity */
    if (position >= t->taille_max)
    {
        majtaille_TabD(t, position + 1);
    }

    /* Shift elements right to open a slot if the target position is occupied */
    if (position < t->taille && t->zone[position] != NULL)
    {
        /* Ensure there is room for one additional element after the shift */
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

    /* Write the element into the now-available slot */
    t->zone[position] = element;

    /* Extend the logical size if writing beyond the current boundary */
    if (position >= t->taille)
    {
        t->taille = position + 1;
    }
}

/*
 * ajoutenDernier_TabD - Append an element at the end of the array
 * @t:       Target dynamic array
 * @element: Pointer to the element to append
 *
 * Convenience wrapper around ajoutElement_TabD() that always inserts
 * at the current logical end (taille), growing the buffer if needed.
 */
void ajoutenDernier_TabD(struct table_D *t, void *element)
{
    ajoutElement_TabD(t, element, t->taille);
}

/*
 * AfficherElement_Tabd - Display the element at the given position via a callback
 * @pf:       Callback function that prints a single element
 * @t:        Source dynamic array
 * @position: Index of the element to display
 *
 * Prints "NULL" if the slot exists but holds a NULL pointer.
 * Reports an error and returns without calling @pf if @t is NULL
 * or @position is out of bounds.
 */
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

/*
 * lireElement_TabD - Read the element at the given position
 * @t:        Source dynamic array
 * @position: Index of the element to read (must be in [0, taille))
 *
 * Returns: The pointer stored at @position, or NULL on error
 *          (NULL pointer, or out-of-bounds index).
 */
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

/*
 * ecrireElement_TabD - Overwrite or insert an element at the given position
 * @t:        Target dynamic array
 * @element:  Pointer to the element to write
 * @position: Index at which to write (must be >= 0)
 *
 * If @position is within the current logical size, the existing pointer
 * is replaced directly (no shifting). If @position is at or beyond the
 * logical size, ajoutElement_TabD() is called to grow and insert instead.
 */
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
        /* Overwrite: replace the existing pointer at this slot */
        t->zone[position] = element;
    }
}

/*
 * rechercheElement_TabD - Find the first element matching a predicate
 * @pf:      Comparison callback; must return non-zero on a match
 * @element: Reference value passed as the second argument to @pf
 * @t:       Array to search (scanned from index 0 to taille-1)
 *
 * NULL slots are skipped and never passed to @pf.
 *
 * Returns: The index of the first matching element, or -1 if not found
 *          or if @t / @pf is NULL.
 */
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

/*
 * echangerElements_TabD - Swap two elements at given indices
 * @t:  Target dynamic array
 * @i:  Index of first element
 * @j:  Index of second element
 */
void echangerElements_TabD(struct table_D *t, int i, int j)
{
    if (t == NULL || i < 0 || j < 0 || i >= t->taille || j >= t->taille)
    {
        ERROR_DEBUG(ERR_INVALID_ARGUMENT, "echangerElements_TabD: invalid indices");
        return;
    }
    
    /* Swap the pointers at the specified positions */
    void *tmp    = t->zone[i];
    t->zone[i]   = t->zone[j];
    t->zone[j]   = tmp;
}