/************************************************************************
 * File    : apprentissage.c
 * =============================================================
 * Description : Implementation of the learning (training) module.
 *               Reads a text corpus from a file, tokenizes it word
 *               by word, maintains a sliding-window context sequence,
 *               and feeds each (context, word) pair into the prediction
 *               tree to build the n-gram language model.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "hash.h"
#include "list.h"
#include "error.h"
#include "sequence.h"
#include "tableau_dyn.h"
#include "arbre_prediction.h"
#include "arbre_io.h"
#include "apprentissage.h"

/*
 * est_fin_de_phrase - Determine whether a token ends a sentence
 * @mot: The token to inspect (may be NULL or empty)
 *
 * Scans the token from the right, skipping any trailing double-quote
 * characters, then tests whether the last remaining character is one
 * of the standard sentence-ending punctuation marks: '.', '!', '?', ':'.
 *
 * Returns: 1 if the token ends a sentence, 0 otherwise.
 */
int est_fin_de_phrase(const char *mot)
{
    if (mot == NULL || mot[0] == '\0')
        return 0;

    int i = (int)strlen(mot) - 1;

    /* Skip trailing double-quote characters before checking punctuation */
    while (i >= 0 && mot[i] == '"')
        i--;

    if (i < 0)
        return 0;

    char dernier = mot[i];
    return (dernier == '.' || dernier == '!' || dernier == '?' || dernier == ':');
}

/*
 * est_debut_de_dialogue - Detect a dialogue dash at the start of a token
 * @mot: The token to inspect (may be NULL or empty)
 *
 * A leading '-' character is used in French literary texts to mark the
 * beginning of a dialogue turn. Detecting it allows the caller to reset
 * the context sequence and prevent cross-dialogue n-gram contamination.
 *
 * Returns: 1 if the token starts with '-', 0 otherwise.
 */
int est_debut_de_dialogue(const char *mot)
{
    if (!mot || strlen(mot) == 0)
        return 0;
    return (mot[0] == '-');
}

/*
 * nettoyer_mot_apprentissage - Normalize a raw token in-place
 * @mot: The token string to clean; modified in place
 *
 * Performs the following normalization steps:
 *   1. Locates the first and last alphanumeric characters, effectively
 *      stripping any leading/trailing punctuation (e.g. quotes, commas).
 *   2. Copies characters in [first_alnum, last_alnum] into a temporary
 *      buffer, retaining only alphanumeric characters, apostrophes ('),
 *      and hyphens (-) — preserving hyphenated and contracted words.
 *   3. Lowercases all alphabetic characters for case-insensitive matching.
 *   4. Writes the result back into @mot.
 *
 * If no alphanumeric character is found, @mot is set to the empty string.
 *
 * Note: @mot must point to a buffer of at least 256 bytes.
 */
void nettoyer_mot_apprentissage(char *mot)
{
    int i, j = 0;
    int debut = -1, fin = -1;
    int len = (int)strlen(mot);
    char temp[256];

    /* Find the index range [debut, fin] of the alphanumeric core */
    for (i = 0; i < len; i++) {
        if (isalnum((unsigned char)mot[i])) {
            if (debut == -1) debut = i;
            fin = i;
        }
    }

    /* No alphanumeric content: clear the token and return */
    if (debut == -1) {
        mot[0] = '\0';
        return;
    }

    /* Copy only valid characters, lowercasing alphabetics */
    for (i = debut; i <= fin; i++) {
        unsigned char c = (unsigned char)mot[i];
        if (isalnum(c))
            temp[j++] = (char)tolower(c);
        else if (c == '\'' || c == '-')
            temp[j++] = c;
    }

    temp[j] = '\0';
    strcpy(mot, temp);
}

/*
 * sequence_reset_manuelle - Break context continuity by flushing the sequence
 * @seq: The context sequence to reset
 * @ht:  Hash table used to intern the empty-string sentinel
 *
 * Overwrites every slot in the circular buffer with the empty-string
 * sentinel (""), effectively erasing the current n-gram context.
 * Called at sentence boundaries and dialogue transitions to prevent
 * the prediction tree from learning spurious cross-sentence n-grams.
 */
void sequence_reset_manuelle(Sequence *seq, struct strhash_table *ht)
{
    if (!seq || !ht)
        return;
    for (int i = 0; i < seq->taille_max; i++)
        sequence_pushWord(seq, "", ht);
}

/*
 * apprendre_fichier - Parse a text file and train the prediction tree
 * @ht:          Hash table used to intern all encountered words
 * @seq:         Sliding-window context sequence (n-gram history buffer)
 * @nom_fichier: Path to the UTF-8 plain-text corpus file
 *
 * Processing pipeline for each whitespace-delimited token:
 *   1. Dialogue detection  — if the token starts with '-', the context
 *      sequence is flushed before processing to avoid cross-dialogue
 *      n-gram leakage.
 *   2. Sentence-end flag   — recorded before cleaning, since punctuation
 *      is stripped by the normalization step.
 *   3. Normalization       — nettoyer_mot_apprentissage() is applied;
 *      empty results (pure punctuation tokens) are discarded.
 *   4. Interning           — the cleaned word is added to the hash table
 *      so all identical strings share a single pointer.
 *   5. Tree update         — the current context is looked up in the
 *      prediction tree and the next word is recorded as a successor.
 *   6. Context advance     — on a sentence boundary the sequence is reset;
 *      otherwise the new word is pushed into the context window.
 *
 * If the file cannot be opened, an empty but valid root node is returned
 * so the caller can still use the (empty) prediction tree safely.
 *
 * Returns: A pointer to the root node of the trained prediction tree,
 *          or NULL if the root node could not be allocated.
 */
struct Noeud *apprendre_fichier(struct strhash_table *ht, Sequence *seq, const char *nom_fichier)
{
    struct Noeud *racine = creerRacine_AP(ht);
    if (!racine) return NULL;

    FILE *f = fopen(nom_fichier, "r");
    if (!f)
    {
        /* File could not be opened; return the empty (valid) root node */
        // fprintf(stderr, "Error: cannot open file %s\n", nom_fichier);
        return racine;
    }

    char tampon[256];
    while (fscanf(f, "%255s", tampon) == 1)
    {
        /* Reset context on dialogue markers to avoid cross-dialogue n-grams */
        if (est_debut_de_dialogue(tampon))
            sequence_reset_manuelle(seq, ht);

        /* Record end-of-sentence flag before normalization strips punctuation */
        int fin = est_fin_de_phrase(tampon);
        nettoyer_mot_apprentissage(tampon);

        if (strlen(tampon) > 0)
        {
            /* Intern the normalized word; abort gracefully on allocation failure */
            const char *mot_intern = strhash_wordAdd(ht, tampon);
            if (!mot_intern) { fclose(f); return racine; }

            /* Look up (or create) the tree node matching the current context,
             * then register the new word as a successor of that context */
            struct Noeud *contexte = rechercher_completer_ngramme_AP(racine, seq);
            if (contexte)
                rechercherMot_AP(contexte, mot_intern);

            /* Advance the context window or reset it at sentence boundaries */
            if (fin)
                sequence_reset_manuelle(seq, ht);
            else
                sequence_pushWord(seq, mot_intern, ht);
        }
    }
    fclose(f);
    return racine;
}