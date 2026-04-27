/************************************************************************
 * File    : tableau_dyn.h
 * =============================================================
 * Description : Public interface for the generic dynamic array module.
 *               Provides a growable buffer of void* pointers with a
 *               configurable multiplicative growth factor.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#ifndef TABLEAU_DYN_H
#define TABLEAU_DYN_H

/*
 * struct table_D - Generic dynamic array backed by a void* pointer zone
 *
 * Stores an ordered sequence of arbitrary pointers. The buffer grows
 * automatically by @facteur whenever the capacity is exceeded.
 * The array does NOT own the elements it points to; callers are
 * responsible for managing the lifetime of stored pointers.
 *
 * @zone:       Heap-allocated array of void* element pointers
 * @taille:     Current number of logically stored elements
 * @taille_max: Current physical capacity of @zone (in number of pointers)
 * @facteur:    Multiplicative growth factor applied on each resize (must be > 0)
 */
struct table_D
{
    void **zone;    /* Storage zone holding element pointers          */
    int taille;     /* Logical size: number of elements currently stored */
    int taille_max; /* Physical capacity: total allocated pointer slots  */
    int facteur;    /* Growth factor used when resizing the buffer       */
};

/* -------------------------------------------------------------------
 * Lifecycle
 * ------------------------------------------------------------------- */

/*
 * creer_TabD - Allocate and initialize a dynamic array
 * @taille_max: Initial buffer capacity in number of elements (clamped to 1 if < 1)
 * @facteur:    Multiplicative growth factor for resizing (must be > 0)
 *
 * Returns: A pointer to the new dynamic array, or NULL on invalid
 *          arguments or allocation failure.
 */
struct table_D *creer_TabD(int taille_max, int facteur);

/*
 * detruire_TabD - Free all resources held by a dynamic array
 * @t: Array to destroy; NULL is tolerated (logs a debug error)
 *
 * Frees the storage zone and the control structure.
 * Elements pointed to by the array are NOT freed.
 */
void detruire_TabD(struct table_D *t);

/* -------------------------------------------------------------------
 * Modification
 * ------------------------------------------------------------------- */

/*
 * majtaille_TabD - Grow the array until its capacity reaches at least @min
 * @t:   Target dynamic array
 * @min: Minimum required capacity after the resize
 *
 * Multiplies taille_max by the growth factor repeatedly until the
 * capacity satisfies @min, then reallocates. New slots are set to NULL.
 * On realloc failure, the existing data is preserved unchanged.
 */
void majtaille_TabD(struct table_D *t, int min);

/*
 * ajoutElement_TabD - Insert an element at @position, shifting right if needed
 * @t:        Target dynamic array
 * @element:  Pointer to store
 * @position: Target index (must be >= 0); buffer is grown if needed
 *
 * If the slot at @position is occupied, all elements from @position
 * onward are shifted right by one before insertion.
 */
void ajoutElement_TabD(struct table_D *t, void *element, int position);

/*
 * ajoutenDernier_TabD - Append an element at the logical end of the array
 * @t:       Target dynamic array
 * @element: Pointer to append
 *
 * Convenience wrapper: equivalent to ajoutElement_TabD(t, element, t->taille).
 */
void ajoutenDernier_TabD(struct table_D *t, void *element);

/*
 * ecrireElement_TabD - Overwrite or insert an element at @position
 * @t:        Target dynamic array
 * @element:  Pointer to write
 * @position: Target index (must be >= 0)
 *
 * If @position is within the current logical size, the existing pointer
 * is replaced in-place (no shifting). Otherwise, ajoutElement_TabD()
 * is called to grow and insert.
 */
void ecrireElement_TabD(struct table_D *t, void *element, int position);

/* -------------------------------------------------------------------
 * Access
 * ------------------------------------------------------------------- */

/*
 * lireElement_TabD - Read the element at @position
 * @t:        Source dynamic array
 * @position: Index to read (must be in [0, taille))
 *
 * Returns: The stored pointer, or NULL if @t is NULL or @position
 *          is out of bounds.
 */
void *lireElement_TabD(const struct table_D *t, int position);

/*
 * rechercheElement_TabD - Find the first element matching a predicate
 * @pf:      Comparison callback; must return non-zero on a match.
 *           Receives (stored_element, @element) as arguments.
 * @element: Reference value passed as the second argument to @pf
 * @t:       Array to search (scanned from index 0 to taille-1)
 *
 * NULL slots are skipped and never passed to @pf.
 *
 * Returns: Index of the first match, or -1 if not found or on error.
 */
int rechercheElement_TabD(int (*pf)(const void *, const void *), const void *element, const struct table_D *t);

/* -------------------------------------------------------------------
 * Display
 * ------------------------------------------------------------------- */

/*
 * AfficherElement_Tabd - Display the element at @position via a callback
 * @pf:       Callback that prints one element; receives the element pointer
 * @t:        Source dynamic array
 * @position: Index of the element to display
 *
 * Prints "NULL" if the slot holds a NULL pointer.
 * Does nothing (and logs an error) if @t is NULL or @position is out of bounds.
 */
void AfficherElement_Tabd(void (*pf)(const void *), const struct table_D *t, int position);

/*
 * AfficherElementChar_TabD - Print a single element cast to char*
 * @element: Pointer to the element to print (cast internally to char*)
 */
void AfficherElementChar_TabD(const void *element);

/*
 * AfficherElementInt_TabD - Print a single element cast to int*
 * @element: Pointer to the element to print (dereferenced as int*)
 */
void AfficherElementInt_TabD(const void *element);


void echangerElements_TabD(struct table_D *t, int i, int j);

#endif /* TABLEAU_DYN_H */