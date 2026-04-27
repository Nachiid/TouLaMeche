/************************************************************************
 * File    : sequence.c
 * =============================================================
 * Description : Contains the function implementations for the
 *               sequence module. A Sequence is a fixed-size
 *               circular buffer of interned string pointers,
 *               backed by a hash table for deduplication.
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
#include "hash.h"
#include "list.h"
#include "sequence.h"
#include "test.h"

/*
 * sequence_creer - Allocate and initialize a new Sequence of capacity n+1
 * @n:  Maximum number of words the sequence can hold (actual size is n+1
 *      to distinguish full from empty in the circular buffer)
 * @ht: Pointer to the hash table used for word interning
 *
 * All slots are pre-filled with the empty string ("") interned in @ht,
 * so every slot is always a valid, non-NULL pointer.
 *
 * Returns: A pointer to the newly allocated Sequence, or NULL on failure.
 */
Sequence *sequence_creer(int n, struct strhash_table *ht)
{
    if (ht == NULL)
        return NULL;

    /* Allocate the Sequence structure itself */
    Sequence *seq = malloc(sizeof(Sequence));
    if (seq == NULL)
    {
        error_print(ERR_ALLOC, "SEQUENCE", "Echec malloc structure Sequence");
        return NULL;
    }

    /* Intern the empty string so all slots share the same sentinel pointer */
    char *adresse_vide = strhash_wordAdd(ht, "");

    /* Initialize fields before allocating the word array */
    seq->taille_max = n + 1;
    seq->position = 0;
    seq->iterateur = 0;

    /* Allocate the word pointer array */
    seq->mots = malloc(seq->taille_max * sizeof(char *));
    if (seq->mots == NULL)
    {
        free(seq);
        return NULL;
    }

    /* Fill every slot with the empty-string sentinel */
    for (int i = 0; i < seq->taille_max; i++)
    {
        seq->mots[i] = adresse_vide;
    }

    return seq;
}

/*
 * sequence_itStart - Reset the iterator to the slot just after the write head
 * @seq: Target sequence
 *
 * Positions the iterator at (position + 1) % taille_max, which is the
 * oldest entry in the circular buffer. Must be called before any
 * sequence_itNext() / sequence_itHasNext() traversal.
 */
void sequence_itStart(Sequence *seq)
{
    seq->iterateur = (seq->position + 1) % (seq->taille_max);
    assert(seq->iterateur < seq->taille_max);
}

/*
 * sequence_itNext - Return the word at the current iterator position and advance
 * @seq: Target sequence
 *
 * Returns the interned string pointer at the current iterator slot, then
 * advances the iterator by one step (wrapping around if needed).
 * Falls back to an empty string on error rather than returning NULL,
 * so callers can always dereference the result safely.
 *
 * Returns: A valid (non-NULL) string pointer, "" on error.
 */
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

/*
 * sequence_itHasNext - Check whether the iterator has not yet reached the write head
 * @seq: Target sequence
 *
 * Returns: 1 if more words remain to be visited, 0 when the iterator
 *          has caught up with the current write position.
 */
int sequence_itHasNext(Sequence *seq)
{
    return (seq->iterateur != seq->position);
}

/*
 * sequence_pushWord - Insert a word at the current write position and advance it
 * @seq:   Target sequence
 * @wordi: The word to intern and store
 * @ht:    Hash table used for word interning
 *
 * Interns @wordi via the hash table (deduplicating storage), stores the
 * resulting pointer at seq->position, then moves the write head forward
 * circularly, overwriting the oldest entry when the buffer is full.
 */
void sequence_pushWord(Sequence *seq, const char *wordi, struct strhash_table *ht)
{
    seq->mots[seq->position] = strhash_wordAdd(ht, wordi);
    seq->position = (seq->position + 1) % (seq->taille_max);
    assert(seq->position < seq->taille_max);
}

/*
 * sequence_nextWord - Peek at the word currently at the write head (not yet overwritten)
 * @seq: Target sequence
 *
 * Returns: The interned string pointer stored at the current write position.
 *          This is the oldest word in the buffer when the buffer is full.
 */
const char *sequence_nextWord(Sequence *seq)
{
    return seq->mots[seq->position];
}

/*
 * sequence_detruire - Free all resources held by a Sequence
 * @seq: Sequence to destroy; safe to call with NULL (logs a debug error)
 *
 * Frees the word pointer array, zeroes all fields defensively to catch
 * use-after-free bugs early, then frees the Sequence structure itself.
 * Note: the interned strings are owned by the hash table and are NOT freed here.
 */
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

    /* Zero out fields before freeing to aid debugging of dangling references */
    seq->taille_max = 0;
    seq->position = 0;
    seq->iterateur = 0;

    free(seq);

    return;
}

/*
 * sequence_print - Print all non-empty words in the sequence to stdout
 * @seq: Target sequence
 *
 * Iterates over the circular buffer from oldest to newest entry.
 * Words that hold the empty-string sentinel (initial fill value) are
 * silently skipped. Non-empty words are separated by " / " and the
 * output is terminated with a newline.
 */
void sequence_print(Sequence *seq)
{
    sequence_itStart(seq);
    int premier_mot_affiche = 0;

    while (sequence_itHasNext(seq))
    {
        const char *mot = sequence_itNext(seq);

        /* Skip empty-string sentinel slots left by initialization */
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

/*
 * sequence_printInTab - Serialize the sequence words into a heap-allocated buffer
 * @seq: Target sequence
 *
 * First pass: computes the exact length needed.
 * Second pass: fills the allocated buffer.
 *
 * The caller is responsible for freeing the returned pointer.
 *
 * Returns: A newly allocated string, or NULL on allocation failure or NULL seq.
 */
char *sequence_printInTab(Sequence *seq)
{
    if (seq == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "sequence_printInTab called with NULL");
        return NULL;
    }

    /* compute total required size */
    size_t total = 1; /* for the final '\0' */
    sequence_itStart(seq);
    while (sequence_itHasNext(seq))
        total += strlen(sequence_itNext(seq)) + 1; /* +1 for the space */

    /* Allocation */
    char *result = malloc(total);
    if (result == NULL)
    {
        error_print(ERR_ALLOC, "SEQUENCE", "Malloc failure in sequence_printInTab");
        return NULL;
    }
    result[0] = '\0';

    /* filling the buffer */
    sequence_itStart(seq);
    while (sequence_itHasNext(seq))
    {
        strcat(result, sequence_itNext(seq));
        strcat(result, " ");
    }

    return result; /* caller must free() */
}