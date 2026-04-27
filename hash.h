/* --------------------------------------------------------------------
 * File    : hash.h
 * --------------------------------------------------------------------
 * Description : Public interface for the string hash table module.
 *               Implements an open-bucket hash table that interns
 *               strings: each unique word is stored exactly once,
 *               and strhash_wordAdd() always returns the same pointer
 *               for identical strings. This property allows O(1)
 *               word comparison by pointer equality throughout the
 *               prediction tree.
 * --------------------------------------------------------------------
 * 22/10/2015   SR   Initial comments
 * --------------------------------------------------------------------
 */

#ifndef _STRHASH_H_
#define _STRHASH_H_

#include "list.h"

/* Default number of buckets. A larger value reduces collision chains
 * at the cost of memory. Tune this to the expected vocabulary size. */
#define ENTREE 10000

/*
 * struct strhash_table - String interning hash table
 *
 * Uses separate chaining: each bucket holds a linked list (keyList)
 * of interned strings that hash to the same slot.
 *
 * @size:   Total number of buckets allocated in @values
 * @values: Array of @size bucket heads, each owning a linked list
 */
struct strhash_table
{
    unsigned int size;
    struct keyList
    {
        s_node *first;      /* Head of the collision chain for this bucket */
        int nbElements;     /* Number of strings stored in this bucket     */
    } *values;
};

/*
 * struct strhash_stat - Occupancy statistics for a hash table
 *
 * Filled by strhash_analyse(). Useful for diagnosing poor hash
 * distribution (high stdDev or nbWordMax >> nbWordMean).
 *
 * @nbWord:     Total number of distinct words stored in the table
 * @nbWordMin:  Size of the smallest bucket (fewest collisions)
 * @nbWordMax:  Size of the largest bucket (most collisions)
 * @nbWordMean: Average number of words per non-empty bucket
 * @stdDev:     Standard deviation of bucket sizes
 */
struct strhash_stat
{
    unsigned int nbWord;
    unsigned int nbWordMin, nbWordMax;
    double nbWordMean, stdDev;
};

/* -------------------------------------------------------------------
 * Lifecycle
 * ------------------------------------------------------------------- */

/*
 * strhash_create - Allocate and initialize a hash table with @size buckets
 * @size: Number of buckets to allocate (use ENTREE or a prime near it)
 *
 * Returns: A pointer to the new hash table, or NULL on failure.
 */
struct strhash_table *strhash_create(int size);

/*
 * strhash_reset - Remove all entries and free the interned strings
 * @ht: Target hash table
 *
 * Frees every interned string stored in the table and resets all
 * bucket heads to empty. The table structure itself remains valid
 * and can be reused after this call.
 */
void strhash_reset(struct strhash_table *ht);

/*
 * strhash_free - Reset the table and free the table structure itself
 * @ht: Hash table to destroy
 *
 * Calls strhash_reset() to release all interned strings, then frees
 * the bucket array and the table structure. @ht must not be used
 * after this call.
 */
void strhash_free(struct strhash_table *ht);

/* -------------------------------------------------------------------
 * Word management
 * ------------------------------------------------------------------- */

/*
 * strhash_wordAdd - Intern a word and return its unique pointer
 * @ht:   Target hash table
 * @word: Null-terminated string to intern
 *
 * If @word is already in the table, the existing pointer is returned
 * without allocating anything. If not, a copy is made, stored in the
 * appropriate bucket, and its pointer is returned.
 *
 * The returned pointer is stable for the lifetime of the table and
 * can be compared by address (pointer equality) instead of strcmp().
 *
 * Returns: The unique interned pointer for @word, or NULL on failure.
 */
char *strhash_wordAdd(struct strhash_table *ht, const char *word);

/*
 * strhash_wordDel - Remove a word from the hash table and free its copy
 * @ht:   Target hash table
 * @word: Null-terminated string to remove
 *
 * WARNING: Any pointer previously returned by strhash_wordAdd() for
 * this word becomes dangling after this call. Only call this if all
 * users of the interned pointer have been updated.
 *
 * Returns: 1 if the word was found and removed, 0 otherwise.
 */
int strhash_wordDel(struct strhash_table *ht, const char *word);

/* -------------------------------------------------------------------
 * Diagnostics
 * ------------------------------------------------------------------- */

/*
 * strhash_display - Print all interned words stored in the table
 * @ht: Source hash table
 *
 * Iterates over every bucket and prints each interned string.
 * Intended for debugging; output order depends on the hash function.
 */
void strhash_display(struct strhash_table *ht);

/*
 * strhash_analyse - Compute occupancy statistics for the hash table
 * @ht:   Source hash table
 * @stat: Output structure filled with the computed statistics
 *
 * Populates @stat with the total word count, min/max bucket sizes,
 * mean bucket occupancy, and standard deviation. Use the results to
 * evaluate hash distribution quality and tune ENTREE if needed.
 */
void strhash_analyse(const struct strhash_table *ht, struct strhash_stat *stat);

#endif /* _STRHASH_H_ */