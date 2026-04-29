/************************************************************************
 * File    : main.c
 * =============================================================
 * Description : Program entry point. Runs the full test suite for all
 *               project modules, then executes the training pipeline:
 *               initializes the data structures, ingests the corpus
 *               files, serializes the resulting prediction tree to disk,
 *               and releases all allocated resources.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "error.h"
#include "hash.h"
#include "list.h"
#include "sequence.h"
#include "tableau_dyn.h"
#include "test.h"
#include "arbre_prediction.h"
#include "arbre_io.h"
#include "apprentissage.h"

int main()
{
    /* ================================================================
     * Phase 1 — Test suite
     * Run every unit and integration test before executing the training
     * pipeline. Any failure aborts the program immediately so that the
     * model is never built on top of broken modules.
     * ================================================================ */

    /* Unit tests for the Sequence (circular buffer) module */
    if (test_sequence())
    {
        ERROR_DEBUG(ERR_SEQUENCE, "Test sequence non reussi");
        return 1;
    }

    /* Unit tests for the dynamic array module */
    if (test_TabD())
    {
        ERROR_DEBUG(ERR_DYNARRAY, "Test Tableau dynamique non reussi");
        return 1;
    }

    /* Unit tests for the prediction tree module */
    if (test_AP())
    {
        ERROR_DEBUG(ERR_TREE, "Test Arbre de prediction non reussi");
        return 1;
    }

    /* Integration test for tree serialization and deserialization */
    if (test_IO())
    {
        ERROR_DEBUG(ERR_TREE, "Test IO non reussi");
        return 1;
    }

    /* Unit tests for the learning (apprentissage) module */
    if (test_apprentissage())
    {
        ERROR_DEBUG(ERR_INTERNAL, "Test d'apprentissage non reussi");
        return 1;
    }

    /* ================================================================
     * Phase 2 — Data structure initialization
     * ================================================================ */

    /* Hash table used to intern all unique words encountered in the corpus.
     * Interning guarantees that identical strings share a single pointer,
     * which both saves memory and enables O(1) pointer-equality comparisons
     * throughout the prediction tree. */
    struct strhash_table *dictionnaire = strhash_create(1000000);

    /* Sliding-window context sequence of depth 100.
     * The hash table is required at creation time to intern the
     * empty-string sentinel that pre-fills every slot. */


     //Problem
     // Need a solution for this => size 100 and the result takes a long time affichage o(n) + a clever way to structur phrases ( average 40 words and then ? )
     // Need a solution for this => size 100 and the result takes a long time affichage o(n) + a clever way to structur phrases ( average 40 words and then ? )
     // Need a solution for this => size 100 and the result takes a long time affichage o(n) + a clever way to structur phrases ( average 40 words and then ? )
     // Need a solution for this => size 100 and the result takes a long time affichage o(n) + a clever way to structur phrases ( average 40 words and then ? )
     // Need a solution for this => size 100 and the result takes a long time affichage o(n) + a clever way to structur phrases ( average 40 words and then ? )
     // Need a solution for this => size 100 and the result takes a long time affichage o(n) + a clever way to structur phrases ( average 40 words and then ? )
    Sequence *seq = sequence_creer(40, dictionnaire);

    /* Root node of the prediction tree.
     * All n-gram paths are anchored to this sentinel node. */
    struct Noeud *racine = creerRacine_AP(dictionnaire);

    /* ================================================================
     * Phase 3 — Training
     * Iterate over the corpus files and feed each one to the learning
     * pipeline. The context sequence is flushed between files to prevent
     * cross-document n-gram contamination.
     * ================================================================ */
    const char *fichiers[] = {"les_miserables_en.txt"};
    for (int i = 0; i < (int)(sizeof(fichiers) / sizeof(fichiers[0])); i++)
    {
        racine = apprendre_fichier(dictionnaire, seq, fichiers[i]);

        /* Reset the context window between files to avoid spurious n-grams
         * that would span the boundary between two independent documents */
        sequence_reset_manuelle(seq, dictionnaire);
    }

    /* ================================================================
     * Phase 4 — Serialization
     * Persist the trained prediction tree to a binary file so it can be
     * loaded at inference time without repeating the training step.
     * ================================================================ */
    printf("\nApprentissage terminé. Sauvegarde dans 'arbre.bin'...\n\n\n\n");
    sauvegarder_AP(racine, "arbre.bin");
    afficherArbre(racine, 0);

    /* ================================================================
     * Phase 5 — Teardown
     * Free every heap-allocated structure in reverse dependency order:
     * tree first (references interned strings), then the sequence,
     * then the hash table (owns the interned strings).
     * ================================================================ */
    detruire_arbre(racine);
    sequence_detruire(seq);
    strhash_free(dictionnaire);

    printf("\n\nFait. L'arbre est prêt pour l'exécution !");

    return 0;
} 


//Problem
//.bin = 16mo : need a smarter way to compress etc ( uint etc)
//.bin = 16mo : need a smarter way to compress etc ( uint etc)
//.bin = 16mo : need a smarter way to compress etc ( uint etc)
//.bin = 16mo : need a smarter way to compress etc ( uint etc)
//.bin = 16mo : need a smarter way to compress etc ( uint etc)
//.bin = 16mo : need a smarter way to compress etc ( uint etc)
//.bin = 16mo : need a smarter way to compress etc ( uint etc)
