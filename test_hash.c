/* --------------------------------------------------------------------
 * File    : test_hash.c
 * --------------------------------------------------------------------
 * Description : Unit and stress tests for the strhash module.
 *               Verifies correct behavior of table creation, word
 *               insertion (including duplicate handling), deletion,
 *               reset, and occupancy statistics. Ends with a
 *               large-scale insertion stress test (1 000 000 words).
 * --------------------------------------------------------------------
 * 15/10/2018   SR   strhash_wordDel rename; cast (const char*) to
 *                   (char*) in wordDel
 * --------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

int main()
{
    struct strhash_stat hstat;
    struct strhash_table *ht;
    const char *mot1 = "hash", *mot2 = "la";

    int i;

    /* --- Test 10: table creation --- */
    if ((ht = strhash_create(1000)) == NULL)
    {
        fprintf(stderr, "10 - strhash_create\n ");
    }

    /* --- Test 20: freshly created table must be empty --- */
    strhash_analyse(ht, &hstat);
    if (hstat.nbWord != 0U)
    {
        fprintf(stderr, "20 - table non vide (%d mots)\n", hstat.nbWord);
    }

    /* --- Test 30: inserting a word must return a pointer to equal content --- */
    if (strcmp(strhash_wordAdd(ht, mot1), mot1))
    {
        fprintf(stderr, "30 - ajout incorrect\n");
    }

    /* --- Test 35: inserting the same word twice must not create a duplicate --- */
    if (strcmp(strhash_wordAdd(ht, mot1), mot1))
    {
        fprintf(stderr, "35 - ajout incorrect (mot en double)\n");
    }

    /* --- Test 40: table must still hold exactly 1 word after double insert --- */
    strhash_analyse(ht, &hstat);
    if (hstat.nbWord != 1U)
    {
        fprintf(stderr, "40 -  %d mots au lieu de 1\n", hstat.nbWord);
    }

    /* --- Test 50: inserting a second distinct word --- */
    if (strcmp(strhash_wordAdd(ht, mot2), mot2))
    {
        fprintf(stderr, "50 - ajout incorrect \n");
    }

    /* --- Test 60: table must now hold exactly 2 words --- */
    strhash_analyse(ht, &hstat);
    if (hstat.nbWord != 2U)
    {
        fprintf(stderr, "60 -  %d mots au lieu de 2\n", hstat.nbWord);
    }

    /* --- Visual check: expected output is "la" then "hash" --- */
    printf("Affichage ateendu :\nla\nhash\n");
    strhash_display(ht);

    /* --- Test 70: deleting mot1 must succeed (return value indicates failure) --- */
    if (strhash_wordDel(ht, mot1))
        fprintf(stderr, "70 - sup incorrect \n");

    /* --- Test 80: table must hold exactly 1 word after deletion --- */
    strhash_analyse(ht, &hstat);
    if (hstat.nbWord != 1U)
    {
        fprintf(stderr, "80 -  %d mots au lieu de 1\n", hstat.nbWord);
    }

    /* --- Stress test: insert 1 000 000 random words and verify each one ---
     * Words are random ASCII strings of length 10–41 characters.
     * A fixed seed (10) is used for reproducibility.
     * Each word is freed after insertion since the table interns its own copy. */
    srand(10);
    for (i = 0; i < 999999; i++)
    {
        char *word;
        int size = rand() % 32 + 10; /* length in [10, 41] including null terminator */
        int l;

        word = (char *)malloc(size * sizeof(char));
        for (l = 0; l < size - 1; l++)
        {
            word[l] = rand() % (128 - ' ') + ' '; /* printable ASCII range */
        }
        word[l] = '\0';

        if (strcmp(strhash_wordAdd(ht, word), word))
            fprintf(stderr, "95 - erreur d'ajout mot %d\n", i);

        free(word); /* the table owns its interned copy; the local buffer is released */
    }

    /* Print occupancy statistics after the stress test */
    strhash_analyse(ht, &hstat);
    fprintf(stdout, "Stat apres test d'insertion intensif (1000000 de mots)\n");
    fprintf(stdout, "Nb mots %u\nMoyenne par clef %f\nMin %u max %u\n",
            hstat.nbWord, hstat.nbWordMean, hstat.nbWordMin, hstat.nbWordMax);
    fprintf(stdout, "Ecart type %f\n", hstat.stdDev);

    /* --- Test 90: reset must empty the table entirely --- */
    strhash_reset(ht);
    strhash_analyse(ht, &hstat);
    if (hstat.nbWord != 0U)
    {
        fprintf(stderr, "90-  RAZ non reussi (reste %d mots)\n", hstat.nbWord);
    }

    /* --- Test 100: table must be usable again after reset --- */
    if (strcmp(strhash_wordAdd(ht, mot1), mot1))
    {
        fprintf(stderr, "100 - ajout incorrect \n");
    }

    /* --- Test 110: table must hold exactly 1 word after post-reset insert --- */
    strhash_analyse(ht, &hstat);
    if (hstat.nbWord != 1U)
    {
        fprintf(stderr, "110 -  %d mots au lieu de 1\n", hstat.nbWord);
    }

    /* Release all resources; no memory leak should be reported past this point */
    strhash_free(ht);
    fprintf(stderr, "fin du test\n");

    return 0;
}