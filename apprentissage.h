/************************************************************************
 * File    : apprentissage.h
 * =============================================================
 * Description : Public interface for the learning (training) module.
 *               Exposes token-level utilities for normalization and
 *               sentence-boundary detection, as well as the main
 *               corpus ingestion function that builds the prediction tree.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#ifndef APPRENTISSAGE_H
#define APPRENTISSAGE_H

#include "arbre_prediction.h" /* struct Noeud — prediction tree node  */
#include "sequence.h"         /* Sequence     — sliding context window */
#include "hash.h"             /* strhash_table — word interning table  */

/* -------------------------------------------------------------------
 * Token classification
 * ------------------------------------------------------------------- */

/*
 * est_fin_de_phrase - Determine whether a token ends a sentence
 * @mot: Token to inspect
 *
 * Returns: 1 if the last non-quote character is '.', '!', '?' or ':',
 *          0 otherwise.
 */
int est_fin_de_phrase(const char *mot);

/*
 * est_debut_de_dialogue - Detect a dialogue dash at the start of a token
 * @mot: Token to inspect
 *
 * Returns: 1 if the token begins with '-', 0 otherwise.
 */
int est_debut_de_dialogue(const char *mot);

/* -------------------------------------------------------------------
 * Token normalization
 * ------------------------------------------------------------------- */

/*
 * nettoyer_mot_apprentissage - Normalize a raw token in-place
 * @mot: Token to clean; must point to a buffer of at least 256 bytes.
 *
 * Strips leading/trailing non-alphanumeric characters, retains internal
 * apostrophes and hyphens, and lowercases the result.
 * Sets @mot to the empty string if no alphanumeric character is found.
 */
void nettoyer_mot_apprentissage(char *mot);

/* -------------------------------------------------------------------
 * Context management
 * ------------------------------------------------------------------- */

/*
 * sequence_reset_manuelle - Flush the context sequence with empty sentinels
 * @seq: Context window to reset
 * @ht:  Hash table used to intern the empty-string sentinel
 *
 * Overwrites all slots with "", breaking n-gram continuity across
 * sentence or dialogue boundaries.
 */
void sequence_reset_manuelle(Sequence *seq, struct strhash_table *ht);

/* -------------------------------------------------------------------
 * Corpus ingestion
 * ------------------------------------------------------------------- */

/*
 * apprendre_fichier - Parse a text corpus and build the prediction tree
 * @ht:          Hash table used for word interning
 * @seq:         Sliding-window context sequence
 * @nom_fichier: Path to the plain-text corpus file
 *
 * Reads the file token by token, normalizes each word, updates the
 * context sequence, and registers every (context, next-word) pair in
 * the prediction tree. Sentence and dialogue boundaries automatically
 * reset the context to prevent cross-boundary n-gram leakage.
 *
 * Returns: A pointer to the root node of the trained prediction tree,
 *          or NULL if root allocation fails. If the file cannot be
 *          opened, an empty but valid root node is returned.
 */
struct Noeud *apprendre_fichier(struct strhash_table *ht, Sequence *seq, const char *nom_fichier);

#endif /* APPRENTISSAGE_H */