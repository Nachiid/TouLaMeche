/************************************************************************
 * File    : arbre_io.c
 * =============================================================
 * Description : Serialization and deserialization of the prediction
 *               tree to/from a binary file. The tree is written in
 *               a pre-order (node before children) depth-first layout.
 *               The root node is handled separately to avoid storing
 *               its empty-string sentinel in the file.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "error.h"
#include "sequence.h"
#include "tableau_dyn.h"
#include "arbre_prediction.h"

/*
 * Binary record layout per node (pre-order, recursive):
 *
 *   [ int:  word_length  ]   length of the word string in bytes (including '\0'),
 *                            or 0 if the word is NULL / empty
 *   [ char: word[len]    ]   the word string (only present when word_length > 0)
 *   [ int:  occurrences  ]   observation count of this node
 *   [ int:  nb_children  ]   number of children that follow immediately
 *   ... children records (recursive, same layout) ...
 *
 * The root node is a special case: its word field is skipped and
 * replaced with a dummy zero-length marker (see sauvegarder_AP).
 */

/*
 * sauvegarder_rec - Serialize a subtree to @f in pre-order (recursive helper)
 * @n: Root of the subtree to serialize (NULL is silently ignored)
 * @f: Destination binary file, already open for writing
 *
 * Writes word length, word bytes, occurrence count, child count, then
 * recurses into each child. Skips writing the word when it is NULL.
 */
void sauvegarder_rec(struct Noeud *n, FILE *f)
{
    if (n == NULL || f == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "sauvegarder_rec appelee avec un noeud ou un fichier NULL");
        return;
    }

    /* Step 1 — Compute and write the word length (includes the null terminator) */
    int taille = 0;
    if (n->mot != NULL)
    {
        taille = (int)strlen(n->mot) + 1;
    }

    if (fwrite(&taille, sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Echec de l'ecriture de la taille du mot");
        return;
    }

    /* Step 2 — Write the word bytes (skipped when taille == 0) */
    if (taille > 0)
    {
        if (fwrite(n->mot, sizeof(char), (size_t)taille, f) != (size_t)taille)
        {
            error_print(ERR_TREE, "TREE", "Echec de l'ecriture des octets du mot");
            return;
        }
    }

    /* Step 3 — Write the occurrence count */
    if (fwrite(&(n->occurrences), sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Echec de l'ecriture des occurrences");
        return;
    }

    /* Step 4 — Write the number of children */
    if (n->fils == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Le tableau de fils du noeud est NULL dans sauvegarder_rec");
        return;
    }

    int nb_fils = n->fils->taille;
    if (fwrite(&nb_fils, sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Echec de l'ecriture du nombre de fils");
        return;
    }

    /* Step 5 — Recurse into each child */
    for (int i = 0; i < nb_fils; i++)
    {
        struct Noeud *enfant = (struct Noeud *)lireElement_TabD(n->fils, i);
        sauvegarder_rec(enfant, f);
    }
}

/*
 * sauvegarder_AP - Serialize the full prediction tree to a binary file
 * @racine:       Root node of the tree to save
 * @nom_fichier:  Path to the output file (created or overwritten)
 *
 * The root node is handled specially: its word field (always the empty
 * string sentinel) is replaced by a dummy zero-length marker so that
 * charger_AP() can reconstruct the root via creerRacine_AP() without
 * storing the sentinel explicitly. The children of the root (first-level
 * words) are then serialized normally via sauvegarder_rec().
 *
 * Does nothing if @racine or @nom_fichier is NULL, or if the file
 * cannot be opened.
 */
void sauvegarder_AP(struct Noeud *racine, const char *nom_fichier)
{
    if (racine == NULL || nom_fichier == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "sauvegarder_AP appelee avec racine ou nom de fichier NULL");
        return;
    }

    FILE *f = fopen(nom_fichier, "wb");
    if (f == NULL)
    {
        error_print(ERR_TREE, "TREE", "Impossible d'ouvrir le fichier en ecriture binaire");
        return;
    }

    /* Write a zero-length placeholder in place of the root's empty-string word */
    int taille_ignoree = 0;
    if (fwrite(&taille_ignoree, sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Echec ecriture placeholder racine");
        fclose(f);
        return;
    }

    /* Write the root's occurrence count */
    if (fwrite(&(racine->occurrences), sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Echec ecriture occurrences racine");
        fclose(f);
        return;
    }

    /* Write the number of first-level children (top-level dictionary words) */
    if (racine->fils == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tableau de fils de la racine NULL dans sauvegarder_AP");
        fclose(f);
        return;
    }

    int nb_mots_dictionnaire = racine->fils->taille;
    if (fwrite(&nb_mots_dictionnaire, sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Echec ecriture nombre de fils racine");
        fclose(f);
        return;
    }

    /* Serialize each first-level child subtree */
    for (int i = 0; i < nb_mots_dictionnaire; i++)
    {
        struct Noeud *enfant = (struct Noeud *)lireElement_TabD(racine->fils, i);
        sauvegarder_rec(enfant, f);
    }

    fclose(f);
}

/*
 * charger_noeud_recursif - Deserialize one node and its subtree from @f
 * @f:  Source binary file, positioned at the start of a node record
 * @ht: Hash table used to intern the word string read from the file
 *
 * Reads the word length, optionally reads the word and interns it in
 * @ht, then reads the occurrence count and child count before recursing
 * into each child subtree. Using strhash_wordAdd() ensures that
 * identical words across nodes share the same pointer (interning).
 *
 * Returns: The reconstructed node, or NULL if the file read fails.
 */
struct Noeud *charger_noeud_recursif(FILE *f, struct strhash_table *ht)
{
    if (f == NULL || ht == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "charger_noeud_recursif appelee avec arguments NULL");
        return NULL;
    }

    /* Read the word length; a short read signals EOF or a corrupt file */
    int taille_mot;
    if (fread(&taille_mot, sizeof(int), 1, f) != 1)
    {
        /* On ne print pas d'erreur ici car cela arrive normalement en fin de fichier */
        return NULL;
    }

    /* Read and intern the word string when present */
    const char *mot_final = NULL;
    if (taille_mot > 0)
    {
        char *temp = malloc((size_t)taille_mot);
        if (temp == NULL)
        {
            error_print(ERR_ALLOC, "TREE", "Echec malloc tampon mot dans charger_noeud_recursif");
            return NULL;
        }

        if (fread(temp, sizeof(char), (size_t)taille_mot, f) != (size_t)taille_mot)
        {
            error_print(ERR_TREE, "TREE", "Erreur de lecture des octets du mot");
            free(temp);
            return NULL;
        }

        /* Intern the string: identical words share a single pointer in memory */
        mot_final = strhash_wordAdd(ht, temp);
        free(temp);
    }

    /* Reconstruct the node with the interned word pointer */
    struct Noeud *n = creerNoeud_AP(mot_final);
    if (n == NULL)
    {
        /* creerNoeud_AP a deja signale l'erreur ALLOC ou NULL */
        return NULL;
    }

    /* Restore the occurrence count saved during serialization */
    if (fread(&(n->occurrences), sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Erreur lecture occurrences");
        return n; /* On retourne le noeud partiellement rempli ou on pourrait le detruire */
    }

    /* Read and reconstruct each child subtree */
    int nb_fils;
    if (fread(&nb_fils, sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Erreur lecture nombre de fils");
        return n;
    }

    for (int i = 0; i < nb_fils; i++)
    {
        struct Noeud *enfant = charger_noeud_recursif(f, ht);
        if (enfant != NULL)
        {
            ajoutenDernier_TabD(n->fils, enfant);
        }
    }

    return n;
}

/*
 * charger_AP - Deserialize a prediction tree from a binary file
 * @nom_fichier: Path to the binary file produced by sauvegarder_AP()
 * @ht:          Hash table used to intern all word strings during loading
 *
 * Reconstructs the root via creerRacine_AP(), reads and discards the
 * dummy zero-length word marker, then restores the root's occurrence
 * count and rebuilds all first-level children via charger_noeud_recursif().
 *
 * Returns: The reconstructed root node, or NULL if the file cannot be
 *          opened.
 */
struct Noeud *charger_AP(const char *nom_fichier, struct strhash_table *ht)
{
    if (nom_fichier == NULL || ht == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "charger_AP appelee avec arguments NULL");
        return NULL;
    }

    FILE *f = fopen(nom_fichier, "rb");
    if (f == NULL)
    {
        error_print(ERR_TREE, "TREE", "Impossible d'ouvrir le fichier en lecture binaire");
        return NULL;
    }

    /* Recreate the root node with its empty-string sentinel */
    struct Noeud *racine = creerRacine_AP(ht);
    if (racine == NULL)
    {
        fclose(f);
        return NULL;
    }

    /* Skip the dummy zero-length word marker written by sauvegarder_AP() */
    int taille_ignoree;
    if (fread(&taille_ignoree, sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Erreur lecture placeholder racine");
        fclose(f);
        return racine;
    }

    /* Restore the root's occurrence count and child count */
    if (fread(&(racine->occurrences), sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Erreur lecture occurrences racine");
        fclose(f);
        return racine;
    }

    int nb_mots_dictionnaire;
    if (fread(&nb_mots_dictionnaire, sizeof(int), 1, f) != 1)
    {
        error_print(ERR_TREE, "TREE", "Erreur lecture nombre de fils racine");
        fclose(f);
        return racine;
    }

    /* Reconstruct all first-level children and attach them to the root */
    for (int i = 0; i < nb_mots_dictionnaire; i++)
    {
        struct Noeud *enfant = charger_noeud_recursif(f, ht);
        if (enfant != NULL)
        {
            ajoutenDernier_TabD(racine->fils, enfant);
        }
    }

    fclose(f);
    return racine;
}

/*
 * normaliser_mot - Convert every character of @str to lowercase in place
 * @str: Null-terminated string to normalize (modified in place)
 *
 * Uses tolower() with an unsigned char cast to avoid undefined behavior
 * on platforms where char is signed and the input contains non-ASCII bytes.
 */
void normaliser_mot(char *str)
{
    if (str == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "normaliser_mot appelee avec une chaine NULL");
        return;
    }

    for (int i = 0; str[i]; i++)
    {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}