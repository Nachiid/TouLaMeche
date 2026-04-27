/************************************************************************
 * File    : sequence.h
 * =============================================================
 * Description : Public interface for the sequence module.
 *               A Sequence is a fixed-size circular buffer of
 *               interned string pointers representing an N-gram
 *               window over a stream of words.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#ifndef _SEQUENCE_H
#define _SEQUENCE_H

#include "list.h"
#include "hash.h"

/*
 * struct Sequence - Fixed-size circular word buffer (N-gram window)
 *
 * Stores the last N words seen in a text stream. The buffer wraps around
 * circularly, so pushing a new word automatically evicts the oldest one.
 * All string pointers are interned via a hash table; the Sequence itself
 * never owns or copies the string data.
 *
 * @mots:       Array of pointers into the hash table's interned strings
 * @taille_max: Total capacity of the array (N + 1, to separate full/empty)
 * @position:   Current write index (next slot to be overwritten)
 * @iterateur:  Current read index used during iteration
 */
typedef struct
{
  char **mots;    /* Array of pointers to interned words (owned by the hash table) */
  int taille_max; /* Total buffer capacity (N + 1) */
  int position;   /* Current write head index */
  int iterateur;  /* Current iterator index */
} Sequence;

/* -------------------------------------------------------------------
 * Lifecycle
 * ------------------------------------------------------------------- */

/*
 * sequence_creer - Allocate and initialize a Sequence of capacity n+1
 * @n:  Number of words the sequence holds (buffer size is n+1)
 * @ht: Hash table used to intern the empty-string sentinel
 *
 * Returns: A pointer to the new Sequence, or NULL on allocation failure.
 */
Sequence *sequence_creer(int n, struct strhash_table *ht);

/*
 * sequence_detruire - Free all resources held by a Sequence
 * @seq: Sequence to destroy (NULL is tolerated; logs a debug error)
 *
 * Frees the internal word array and the Sequence structure itself.
 * The interned strings remain alive in the hash table.
 */
void sequence_detruire(Sequence *seq);

/* -------------------------------------------------------------------
 * Modification
 * ------------------------------------------------------------------- */

/*
 * sequence_pushWord - Push a new word into the sequence
 * @seq:   Target sequence
 * @wordi: Word to intern and store at the current write position
 * @ht:    Hash table used for word interning
 *
 * Advances the write head circularly after insertion, overwriting the
 * oldest entry when the buffer is full.
 */
void sequence_pushWord(Sequence *seq, const char *wordi, struct strhash_table *ht);

/* -------------------------------------------------------------------
 * Iterator
 * ------------------------------------------------------------------- */

/*
 * sequence_itStart - Reset the iterator to the oldest entry in the buffer
 * @seq: Target sequence
 *
 * Must be called before any sequence_itNext() / sequence_itHasNext() loop.
 */
void sequence_itStart(Sequence *seq);

/*
 * sequence_itHasNext - Check whether the iterator has more words to visit
 * @seq: Target sequence
 *
 * Returns: 1 if the iterator has not yet reached the write head, 0 otherwise.
 */
int sequence_itHasNext(Sequence *seq);

/*
 * sequence_itNext - Return the word at the iterator position and advance it
 * @seq: Target sequence
 *
 * Returns: The interned string at the current iterator slot, then steps
 *          forward. Returns "" on error; never returns NULL.
 */
const char *sequence_itNext(Sequence *seq);

/*
 * sequence_nextWord - Peek at the word currently at the write head
 * @seq: Target sequence
 *
 * Returns: The interned string stored at the current write position.
 *          This is the oldest word when the buffer is full.
 */
const char *sequence_nextWord(Sequence *seq);

/* -------------------------------------------------------------------
 * Display
 * ------------------------------------------------------------------- */

/*
 * sequence_print - Print all non-empty words in the sequence to stdout
 * @seq: Target sequence
 *
 * Empty-string sentinel slots are skipped. Non-empty words are separated
 * by " / " and the output is terminated with a newline.
 */
void sequence_print(Sequence *seq);

/*
 * sequence_printInTab - Serialize the sequence into a heap-allocated string
 * @seq: Target sequence
 *
 * Returns: A newly allocated space-separated string. 
 *          The CALLER must free() the result. Returns NULL on error.
 */
char *sequence_printInTab(Sequence *seq);

#endif /* _SEQUENCE_H */