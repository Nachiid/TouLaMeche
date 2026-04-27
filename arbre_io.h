/************************************************************************
 * File    : arbre_io.h
 * =============================================================
 * Description : Public interface for the prediction tree
 *               serialization module. Provides binary save/load
 *               of the full N-gram trie, along with a word
 *               normalization utility.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#ifndef _ARBRE_IO_H
#define _ARBRE_IO_H

#include "list.h"
#include "hash.h"
#include "arbre_prediction.h"
#include <stdio.h>

/* -------------------------------------------------------------------
 * Serialization
 * ------------------------------------------------------------------- */

/*
 * sauvegarder_AP - Serialize the full prediction tree to a binary file
 * @racine:      Root node of the tree to save
 * @nom_fichier: Path to the output file (created or overwritten)
 *
 * The root's empty-string sentinel word is replaced by a dummy
 * zero-length marker so the loader can recreate the root via
 * creerRacine_AP() without storing it explicitly. Children are
 * written recursively via sauvegarder_rec() in pre-order.
 *
 * Does nothing if @racine or @nom_fichier is NULL, or if the file
 * cannot be opened.
 */
void sauvegarder_AP(struct Noeud *racine, const char *nom_fichier);

/*
 * sauvegarder_rec - Recursive helper: serialize one subtree to @f
 * @n: Root of the subtree (NULL is silently ignored)
 * @f: Destination binary file, already open in "wb" mode
 *
 * Writes word length, word bytes, occurrence count, child count,
 * then recurses into each child in index order. Not intended to be
 * called directly — use sauvegarder_AP() instead.
 */
void sauvegarder_rec(struct Noeud *n, FILE *f);

/* -------------------------------------------------------------------
 * Deserialization
 * ------------------------------------------------------------------- */

/*
 * charger_AP - Deserialize a prediction tree from a binary file
 * @nom_fichier: Path to a file produced by sauvegarder_AP()
 * @ht:          Hash table used to intern all word strings on load
 *
 * Reconstructs the root via creerRacine_AP(), restores its metadata,
 * then rebuilds the full subtree via charger_noeud_recursif().
 * All word strings are interned through @ht so pointer-equality
 * comparisons (comparerMot_AP) remain valid after loading.
 *
 * Returns: The reconstructed root node, or NULL if the file cannot
 *          be opened.
 */
struct Noeud *charger_AP(const char *nom_fichier, struct strhash_table *ht);

/* -------------------------------------------------------------------
 * Utilities
 * ------------------------------------------------------------------- */

/*
 * normaliser_mot - Convert a word to lowercase in place
 * @str: Null-terminated string to normalize (modified in place)
 *
 * Uses tolower() with an unsigned char cast to avoid undefined
 * behavior on platforms where char is signed and the input contains
 * non-ASCII bytes. Call before inserting or looking up words to
 * ensure case-insensitive consistency across the tree.
 */
void normaliser_mot(char *str);

#endif /* _ARBRE_IO_H */