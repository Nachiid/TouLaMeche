/************************************************************************
 * File    : arbre_prediction.c
 * =============================================================
 * Description : Implements the prediction tree module.
 *               The tree is an N-ary trie where each path from root
 *               to a node encodes an N-gram. Child arrays are kept
 *               sorted in descending order of occurrence count so
 *               that the most probable next word is always at index 0.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "sequence.h"
#include "tableau_dyn.h"
#include "arbre_prediction.h"

/*
 * creerNoeud_AP - Allocate and initialize a new tree node
 * @mot: Interned string pointer owned by the hash table (must not be NULL)
 *
 * The child array is initialized with capacity 2 and a growth factor of 2,
 * which is intentionally small since most nodes have few children.
 * The error path from creer_TabD() is already reported internally;
 * this function only needs to clean up the partially built node.
 *
 * Returns: A pointer to the new node with occurrences set to 1,
 *          or NULL on failure.
 */
struct Noeud *creerNoeud_AP(const char *mot)
{
    if (mot == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tentative de creer un noeud avec un mot NULL");
        return NULL;
    }

    struct Noeud *mon_noeud = (struct Noeud *)malloc(sizeof(struct Noeud));
    if (mon_noeud == NULL)
    {
        error_print(ERR_ALLOC, "TREE", "Echec malloc structure Noeud");
        return NULL;
    }

    mon_noeud->mot = mot;
    mon_noeud->occurrences = 1;

    /* Start with a small child array; it will grow on demand */
    mon_noeud->fils = creer_TabD(2, 2);
    if (mon_noeud->fils == NULL)
    {
        /* creer_TabD already reported the error; just release the node */
        free(mon_noeud);
        return NULL;
    }

    return mon_noeud;
}

/*
 * creerRacine_AP - Create and initialize the root node of the prediction tree
 * @ht: Hash table used to intern the empty-string sentinel word
 *
 * The root represents the "start of sequence" context and holds the
 * empty string as its word. Its occurrence count is set to 0 since
 * it is never a prediction candidate itself. The child array starts
 * with capacity 5 to accommodate the most frequent first words.
 *
 * Returns: A pointer to the root node, or NULL on allocation failure.
 */
struct Noeud *creerRacine_AP(struct strhash_table *ht)
{
    struct Noeud *racine = (struct Noeud *)malloc(sizeof(struct Noeud));
    if (racine == NULL)
    {
        error_print(ERR_ALLOC, "TREE", "Echec malloc racine");
        return NULL;
    }

    racine->mot = strhash_wordAdd(ht, "");
    racine->occurrences = 0;
    racine->fils = creer_TabD(5, 2);

    if (racine->fils == NULL)
    {
        free(racine);
        return NULL;
    }

    return racine;
}

/*
 * rechercher_completer_ngramme_AP - Walk the tree along a sequence, creating missing nodes
 * @racine: Root of the prediction tree
 * @seq:    Sequence representing the N-gram path to follow
 *
 * Iterates over every word in @seq. At each step, if a matching child
 * exists the traversal descends into it; otherwise a new child node is
 * created on the fly. This ensures every N-gram seen during training has
 * a corresponding path in the tree.
 *
 * Returns: The node reached after consuming the full sequence (i.e. the
 *          node whose children are the valid next-word predictions), or
 *          NULL if @racine or @seq is NULL.
 */
struct Noeud *rechercher_completer_ngramme_AP(struct Noeud *racine, Sequence *seq)
{
    if (racine == NULL || seq == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Racine ou Sequence NULL dans la recherche de n-gramme");
        return NULL;
    }

    struct Noeud *noeud_courant = racine;
    sequence_itStart(seq);

    while (sequence_itHasNext(seq))
    {
        const char *mot = sequence_itNext(seq);
        int index = rechercheElement_TabD(comparerMot_AP, mot, noeud_courant->fils);

        if (index != -1)
        {
            noeud_courant = (struct Noeud *)lireElement_TabD(noeud_courant->fils, index);
        }
        else
        {
            /* Path does not exist yet — extend the tree for this N-gram */
            ajouterMot_AP(noeud_courant, mot);
            /* The new node is always appended last */
            int dernier = noeud_courant->fils->taille - 1;
            noeud_courant = (struct Noeud *)lireElement_TabD(noeud_courant->fils, dernier);
        }
    }

    return noeud_courant;
}

/*
 * ajouterMot_AP - Add a new word as a child of @mon_noeud
 * @mon_noeud: Parent node that will own the new child
 * @mot:       Interned string to store in the new child node
 *
 * Creates a new node via creerNoeud_AP() and appends it to the parent's
 * child array. If node creation fails the parent remains unchanged.
 */
void ajouterMot_AP(struct Noeud *mon_noeud, const char *mot)
{
    if (mon_noeud == NULL || mot == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Argument NULL dans ajouterMot_AP");
        return;
    }

    struct Noeud *nouveau = creerNoeud_AP(mot);
    if (nouveau != NULL)
    {
        ajoutenDernier_TabD(mon_noeud->fils, nouveau);
    }
}

/*
 * rechercherMot_AP - Record an observed next word for training (learn step)
 * @noeud_final: Node whose children represent possible next words
 * @mot:         The word that was observed following the current N-gram context
 *
 * If @mot is already a child of @noeud_final, its occurrence count is
 * incremented and the child array is re-sorted to maintain descending
 * order (so the best prediction stays at index 0 in O(1) lookup time).
 * If @mot is not yet a child, a new child node is created with count 1.
 */
void rechercherMot_AP(struct Noeud *noeud_final, const char *mot)
{
    if (noeud_final == NULL || mot == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Argument NULL dans rechercherMot_AP");
        return;
    }

    int index = rechercheElement_TabD(comparerMot_AP, mot, noeud_final->fils);

    if (index != -1)
    {
        struct Noeud *cible = (struct Noeud *)lireElement_TabD(noeud_final->fils, index);
        cible->occurrences++;
        /* Bubble the updated node upward to keep the array sorted (descending) */
        remonterNoeud_AP(noeud_final->fils, index);
    }
    else
    {
        ajouterMot_AP(noeud_final, mot);
    }
}

/*
 * MotMaxOccurrences_AP - Return the most probable next-word prediction
 * @noeud_courant: Node whose children are the prediction candidates
 *
 * Because remonterNoeud_AP() maintains the child array in descending
 * occurrence order, the best candidate is always at index 0, making
 * this lookup O(1).
 *
 * Returns: The child node with the highest occurrence count, or NULL
 *          if @noeud_courant is NULL or has no children.
 */
struct Noeud *MotMaxOccurrences_AP(struct Noeud *noeud_courant)
{
    if (noeud_courant == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "noeud_courant NULL dans MotMaxOccurrences_AP");
        return NULL;
    }

    if (noeud_courant->fils == NULL || noeud_courant->fils->taille == 0)
    {
        return NULL;
    }

    /* The child array is kept sorted: the highest-occurrence node is at index 0 */
    return (struct Noeud *)lireElement_TabD(noeud_courant->fils, 0);
}

/*
 * comparerMot_AP - Predicate for rechercheElement_TabD: match a node by word pointer
 * @noeud_v: void* cast of a (struct Noeud*) stored in the child array
 * @mot_v:   void* cast of the interned string pointer to look for
 *
 * Uses pointer equality rather than strcmp() because all strings are
 * interned in the hash table — identical words always share the same address.
 *
 * Returns: 1 if the node's word pointer equals @mot_v, 0 otherwise.
 */
int comparerMot_AP(const void *noeud_v, const void *mot_v)
{
    const struct Noeud *noeud = (const struct Noeud *)noeud_v;
    const char *mot_cherche = (const char *)mot_v;

    /* Pointer comparison is valid here: interned strings are deduplicated */
    return (noeud->mot == mot_cherche);
}

/*
 * detruire_arbre - Recursively free a subtree rooted at @noeud
 * @noeud: Root of the subtree to destroy (NULL is safe and returns immediately)
 *
 * Performs a post-order traversal: children are destroyed before their
 * parent. The child dynamic array is freed via detruire_TabD() after all
 * children have been recursed into.
 *
 * NOTE: noeud->mot is intentionally NOT freed — string ownership belongs
 *       to the hash table, not to the tree nodes.
 */
void detruire_arbre(struct Noeud *noeud)
{
    if (noeud == NULL)
    {
        return;
    }

    if (noeud->fils != NULL)
    {
        for (int i = 0; i < noeud->fils->taille; i++)
        {
            struct Noeud *fils_courant = (struct Noeud *)lireElement_TabD(noeud->fils, i);
            detruire_arbre(fils_courant);
        }
        detruire_TabD(noeud->fils);
    }

    /* Do NOT free noeud->mot: strings are owned by the hash table */
    noeud->mot = NULL;
    free(noeud);
}

/*
 * remonterNoeud_AP - Bubble a node up to restore descending occurrence order
 * @fils:  Child array of the parent node
 * @index: Position of the node whose occurrence count was just incremented
 *
 * Performs an insertion-sort step: swaps the node at @index with its
 * predecessor as long as its occurrence count is strictly greater.
 * This keeps the child array sorted so MotMaxOccurrences_AP() runs in O(1).
 */
void remonterNoeud_AP(struct table_D *fils, int index)
{
    if (fils == NULL) return;

    while (index > 0)
    {
        struct Noeud *courant  = (struct Noeud *)lireElement_TabD(fils, index);
        struct Noeud *precedent = (struct Noeud *)lireElement_TabD(fils, index - 1);

        if (courant->occurrences > precedent->occurrences)
        {
            echangerElements_TabD(fils, index, index - 1);
            index--;
        }
        else
        {
            break;
        }
    }
}

/*
 * afficherMot_AP - Print a node's word and occurrence count (callback-compatible)
 * @noeud_v: void* cast of a (struct Noeud*) to display
 *
 * Output format: "<word> (<count>)\n"
 * Silently does nothing if @noeud_v or its word pointer is NULL.
 */
void afficherMot_AP(const void *noeud_v)
{
    const struct Noeud *n = (const struct Noeud *)noeud_v;
    if (n != NULL && n->mot != NULL)
    {
        printf("%s (%d)\n", n->mot, n->occurrences);
    }
}

/*
 * afficherArbre - Recursively print the prediction tree with indentation
 * @racine:  Root of the subtree to display
 * @hauteur: Current depth level, used to compute the indentation (2 spaces per level)
 *
 * Performs a pre-order traversal: the node is printed before its children.
 * Call with hauteur = 0 to display the full tree from the root.
 */
void afficherArbre(const struct Noeud *racine, int hauteur)
{
    if (racine == NULL) return;

    for (int i = 0; i < hauteur; i++) printf("  ");
    afficherMot_AP(racine);

    if (racine->fils != NULL)
    {
        for (int i = 0; i < racine->fils->taille; i++)
        {
            struct Noeud *f = (struct Noeud *)lireElement_TabD(racine->fils, i);
            afficherArbre(f, hauteur + 1);
        }
    }
}