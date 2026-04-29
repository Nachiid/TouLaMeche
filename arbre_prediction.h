/************************************************************************
 * File    : arbre_prediction.h
 * =============================================================
 * Description : Public interface for the prediction tree module.
 *               The tree is an N-ary trie where each root-to-node path
 *               encodes an N-gram context. Child arrays are maintained
 *               in descending occurrence order so the best prediction
 *               is always retrievable in O(1).
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#ifndef _ARBRE_PREDICTION_H
#define _ARBRE_PREDICTION_H

#include "list.h"          /* Linked-list utilities                  */
#include "tableau_dyn.h"   /* Generic dynamic array (struct table_D) */
#include "hash.h"             // Pour struct strhash_table
#include "sequence.h"         // Pour Sequence


/*
 * struct Noeud - Single node in the prediction tree
 *
 * Each node represents one word in an N-gram path. A node's children
 * are the words that were observed to follow the current context during
 * training. Children are stored in a dynamic array sorted by @occurrences
 * in descending order so that the most probable next word is at index 0.
 *
 * String ownership: @mot is an interned pointer into the hash table and
 * must NEVER be freed by the tree.
 *
 * @mot:         Interned string pointer representing the word at this node
 * @occurrences: Number of times this word was observed in its N-gram context
 * @fils:        Dynamic array of (struct Noeud*) children, sorted descending
 */
struct Noeud {
    const char *mot;         /* Interned word string (owned by the hash table, not the node) */
    int occurrences;         /* Observation count used for ranking predictions               */
    struct table_D *fils;    /* Children nodes: possible next words for this context         */
};

/* -------------------------------------------------------------------
 * Lifecycle
 * ------------------------------------------------------------------- */

/*
 * creerNoeud_AP - Allocate and initialize a new tree node
 * @mot: Interned string pointer (must not be NULL)
 *
 * Initializes @occurrences to 1 and allocates an empty child array
 * with initial capacity 2 and growth factor 2.
 *
 * Returns: A pointer to the new node, or NULL on failure.
 */
struct Noeud *creerNoeud_AP(const char *mot);

/*
 * creerRacine_AP - Create the root node of the prediction tree
 * @ht: Hash table used to intern the empty-string sentinel
 *
 * The root holds the empty string and has @occurrences set to 0 since
 * it is never a prediction candidate. Its child array starts with
 * capacity 5 to reduce early reallocations.
 *
 * Returns: A pointer to the root node, or NULL on allocation failure.
 */
struct Noeud *creerRacine_AP(struct strhash_table *ht);

/*
 * detruire_arbre - Recursively free a subtree (post-order traversal)
 * @noeud: Root of the subtree to destroy; NULL is safe and returns immediately
 *
 * Frees every node and its child array. Does NOT free the interned
 * strings — those are owned by the hash table.
 */
void detruire_arbre(struct Noeud *noeud);

/* -------------------------------------------------------------------
 * Training
 * ------------------------------------------------------------------- */

/*
 * rechercher_completer_ngramme_AP - Walk the trie along @seq, creating missing nodes
 * @racine: Root of the prediction tree
 * @seq:    Sequence encoding the N-gram path to follow
 *
 * At each step, if a matching child exists the traversal descends into
 * it; otherwise a new child node is created. Guarantees that every
 * N-gram seen during training has a corresponding path in the tree.
 *
 * Returns: The node reached after consuming the full sequence, or NULL
 *          if @racine or @seq is NULL.
 */
struct Noeud *rechercher_completer_ngramme_AP(struct Noeud *racine, Sequence *seq);

/*
 * rechercherMot_AP - Record an observed next word (learn step)
 * @noeud_final: Context node whose children are the next-word candidates
 * @mot:         The word observed after this N-gram context
 *
 * If @mot is already a child, its @occurrences is incremented and the
 * child array is re-sorted via remonterNoeud_AP(). If not, a new child
 * node is created with @occurrences = 1.
 */
void rechercherMot_AP(struct Noeud *noeud_final, const char *mot);

/*
 * ajouterMot_AP - Create a new child node and append it to @mon_noeud
 * @mon_noeud: Parent node receiving the new child
 * @mot:       Interned string for the new child node
 */
void ajouterMot_AP(struct Noeud *mon_noeud, const char *mot);

/*
 * remonterNoeud_AP - Bubble a node up to restore descending occurrence order
 * @fils:  Child array containing the node to reposition
 * @index: Position of the node whose @occurrences was just incremented
 *
 * Performs a single insertion-sort step so that MotMaxOccurrences_AP()
 * can always read the best prediction from index 0 in O(1).
 */
void remonterNoeud_AP(struct table_D *fils, int index);

/* -------------------------------------------------------------------
 * Prediction
 * ------------------------------------------------------------------- */

/*
 * MotMaxOccurrences_AP - Return the most probable next-word prediction
 * @noeud_courant: Context node whose children are the candidates
 *
 * Exploits the invariant maintained by remonterNoeud_AP(): the child
 * with the highest occurrence count is always at index 0.
 *
 * Returns: The child node with the highest @occurrences, or NULL if
 *          @noeud_courant is NULL or has no children.
 */
struct Noeud *MotMaxOccurrences_AP(struct Noeud *noeud_courant);

/* -------------------------------------------------------------------
 * Utilities
 * ------------------------------------------------------------------- */

/*
 * comparerMot_AP - Predicate for rechercheElement_TabD: match a node by word pointer
 * @noeud_courant_void: void* of a (struct Noeud*) stored in a child array
 * @mot:                void* of the interned string pointer to search for
 *
 * Uses pointer equality (not strcmp) because interned strings are
 * deduplicated — identical words always share the same address.
 *
 * Returns: 1 if the node's word matches @mot, 0 otherwise.
 */
int comparerMot_AP(const void *noeud_courant_void, const void *mot);

/*
 * afficherMot_AP - Print a node's word and occurrence count (callback-compatible)
 * @mon_noeud: void* cast of a (struct Noeud*) to display
 *
 * Output format: "<word> (<count>)\n". Safe to pass directly as a
 * callback to AfficherElement_Tabd(). Does nothing if the pointer is NULL.
 */
void afficherMot_AP(const void *mon_noeud);

/*
 * afficherArbre - Recursively print the prediction tree with indentation
 * @racine: Root of the subtree to display
 * @niveau: Current depth, controls indentation (2 spaces per level)
 *
 * Performs a pre-order traversal. Call with niveau = 0 to print the
 * full tree starting from the root.
 */
void afficherArbre(const struct Noeud *racine, int niveau);

#endif /* _ARBRE_PREDICTION_H */