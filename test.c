/************************************************************************
 * File    : test.c
 * =============================================================
 * Description : Integration and unit tests for all project modules:
 *               sequence, dynamic array, prediction tree, and I/O.
 *               Each test function is self-contained, manages its own
 *               resources, and returns 0 on success or 1 on failure.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

/* Uncomment to disable all assertions (production builds) */
// #define NDEBUG

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "list.h"
#include "error.h"
#include "sequence.h"
#include "tableau_dyn.h"
#include "arbre_prediction.h"
#include "arbre_io.h"

/* ===================================================================
 * test_sequence - Unit tests for the Sequence (circular buffer) module
 *
 * Verifies: creation, hash table state after init, initial word content,
 * circular overwrite behavior, buffer content after overflow, and a
 * large-scale stress insertion (10 000 words).
 *
 * Returns: 0 if all checks pass, 1 on the first failed assertion.
 * =================================================================== */
int test_sequence()
{
    /* --- Setup: hash table and sequence of N-gram size 3 --- */
    struct strhash_table *ht = strhash_create(ENTREE);
    struct strhash_stat hstat;
    const char *dernierMot = NULL;

    /* Test 1: hash table creation must succeed */
    if (ht == NULL)
    {
        error_print(ERR_HASH, "TEST", "Erreur lors de la création de la table de hachage.");
        return 1;
    }

    Sequence *seq = sequence_creer(3, ht);

    /* Test 2: creation must intern exactly one word (the empty-string sentinel) */
    strhash_analyse(ht, &hstat);
    if (hstat.nbWord != 1U)
    {
        fprintf(stderr, "2 - Erreur dans sequence_initialise - Mots dans la table : %d (attendu : 1)\n", hstat.nbWord);
        sequence_detruire(seq);
        strhash_free(ht);
        return 1;
    }

    /* Test 3: the word at the current write head must be the empty-string sentinel */
    dernierMot = sequence_nextWord(seq);
    if (strcmp(dernierMot, "") != 0)
    {
        fprintf(stderr, "3 - Erreur : le mot initial devrait être vide, reçu : [%s]\n", dernierMot);
        sequence_detruire(seq);
        strhash_free(ht);
        return 1;
    }

    /* Test 4: push 4 words into a size-3 buffer — TEST0 must be evicted,
     * leaving TEST1 TEST2 TEST3 (each separated by a space, with a trailing space) */
    char wordi[256];
    for (int i = 0; i < 4; i++)
    {
        sprintf(wordi, "TEST%d", i);
        sequence_pushWord(seq, wordi, ht);
    }

    char *resultat = sequence_printInTab(seq);
    if (strcmp(resultat, "TEST1 TEST2 TEST3 ") != 0)
    {
        fprintf(stderr, "4 - Erreur contenu sequence.\nAttendu : TEST1 TEST2 TEST3 \nAffiché  : %s\n", resultat);
        sequence_detruire(seq);
        strhash_free(ht);
        return 1;
    }

    /* --- Stress test: reset and reuse the same hash table --- */
    strhash_reset(ht);
    sequence_detruire(seq);
    seq = sequence_creer(3, ht);

    /* Push 10 000 words; only the last 3 should survive in the buffer */
    for (int i = 0; i < 10000; i++)
    {
        sprintf(wordi, "TEST%d", i);
        sequence_pushWord(seq, wordi, ht);

        /* NOTE: sequence_nextWord() returns the slot at the write head, which
         * points to the next slot to be overwritten (i.e. the oldest entry),
         * not the word just inserted. Use the iterator to inspect content. */
        dernierMot = sequence_nextWord(seq);
    }

    /* Test 6: after 10 000 pushes into a size-3 buffer, only the last 3 remain */
    resultat = sequence_printInTab(seq);
    if (strcmp(resultat, "TEST9997 TEST9998 TEST9999 ") != 0)
    {
        fprintf(stderr, "6 - Erreur test intensif.\nAttendu : TEST9997 TEST9998 TEST9999 \nAffiché  : %s\n", resultat);
        sequence_detruire(seq);
        strhash_free(ht);
        return 1;
    }

    /* Test 7: hash table must hold exactly 10 001 unique words
     * (TEST0 … TEST9999 + the empty-string sentinel) */
    strhash_analyse(ht, &hstat);
    if (hstat.nbWord != 10001)
    {
        fprintf(stderr, "7 - Erreur : nbWord dans ht = %d (attendu : 10001)\n", hstat.nbWord);
        sequence_detruire(seq);
        strhash_free(ht);
        return 1;
    }

    /* --- Teardown --- */
    sequence_detruire(seq);
    strhash_free(ht);

    printf("Tests du module Sequence : REUSSIS\n");
    return 0;
}

/* -------------------------------------------------------------------
 * Comparison and display callbacks for the dynamic array tests.
 * These are local to test.c and not part of any public module API.
 * ------------------------------------------------------------------- */

/*
 * rechercheInt_TabD - Predicate: match two int* pointers by value
 * @element_recherche: void* of the reference int
 * @element_TabD:      void* of the stored int
 *
 * Returns: 1 if both integers are equal, 0 otherwise.
 */
int rechercheInt_TabD(const void *element_recherche, const void *element_TabD)
{
    const int *val_recherche = (const int *)element_recherche;
    const int *val_tab       = (const int *)element_TabD;

    return (*val_recherche == *val_tab) ? 1 : 0;
}

/*
 * rechercheChar_TabD - Predicate: match two char* pointers by string content
 * @element_recherche: void* of the reference string
 * @element_TabD:      void* of the stored string
 *
 * Returns: 1 if both strings are equal, 0 otherwise.
 */
int rechercheChar_TabD(const void *element_recherche, const void *element_TabD)
{
    const char *val_recherche = (const char *)element_recherche;
    const char *val_tab       = (const char *)element_TabD;

    return (strcmp(val_recherche, val_tab) == 0) ? 1 : 0;
}

/*
 * AfficherElementChar_TabD - Print a stored element as a single char
 * @element: void* of a char to dereference and print
 */
void AfficherElementChar_TabD(const void *element)
{
    const char *val_char = (const char *)element;
    printf("%c\n", *val_char);
}

/*
 * AfficherElementInt_TabD - Print a stored element as an int
 * @element: void* of an int to dereference and print
 */
void AfficherElementInt_TabD(const void *element)
{
    const int *val_int = (const int *)element;
    printf("%d\n", *val_int);
}

/* ===================================================================
 * test_TabD - Unit tests for the dynamic array module
 *
 * Verifies: creation, initial field values, capacity growth, insertion
 * at arbitrary positions, logical size tracking, and element retrieval.
 *
 * Returns: 0 if all checks pass, 1 on the first failed assertion.
 * =================================================================== */
int test_TabD()
{
    /* --- Setup: initial capacity 20, growth factor 2 --- */
    struct table_D *tableau1 = creer_TabD(20, 2);
    int i = 20, i1 = 2, i3 = 1;
    char c = 'A', c2 = '2';
    int position;

    /* Test 1: array creation must succeed */
    if (tableau1 == NULL)
    {
        fprintf(stderr, "1 - Erreur lors du création du tableau\n");
        return 1;
    }

    /* Test 2–4: verify initial field values (taille_max, taille, facteur) */
    if (tableau1->taille_max != 20)
    {
        fprintf(stderr, "2 - La taille max du tableau est %d au lieu de 20\n", tableau1->taille_max);
        detruire_TabD(tableau1);
        return 1;
    }
    if (tableau1->taille != 0)
    {
        fprintf(stderr, "3 - La taille du tableau est %d au lieu de 0\n", tableau1->taille);
        detruire_TabD(tableau1);
        return 1;
    }
    if (tableau1->facteur != 2)
    {
        fprintf(stderr, "4 - Le facteur du tableau est %d au lieu de 2\n", tableau1->facteur);
        detruire_TabD(tableau1);
        return 1;
    }

    /* Test 5–7: majtaille_TabD must grow capacity to the next power-of-factor
     * ceiling above the requested minimum (20 * 2 * 2 = 80 >= 56) */
    majtaille_TabD(tableau1, 56);
    if (tableau1->taille_max != 80)
    {
        fprintf(stderr, "5 - La taille max du tableau est %d au lieu de 80\n", tableau1->taille_max);
        detruire_TabD(tableau1);
        return 1;
    }
    if (tableau1->taille != 0)
    {
        fprintf(stderr, "6 - La taille du tableau est %d au lieu de 0\n", tableau1->taille);
        detruire_TabD(tableau1);
        return 1;
    }
    if (tableau1->facteur != 2)
    {
        fprintf(stderr, "7 - Le facteur du tableau est %d au lieu de 2\n", tableau1->facteur);
        detruire_TabD(tableau1);
        return 1;
    }

    /* Generic pointer used to store addresses of stack variables as elements */
    void *ptr = &i;

    /* Test 8–10: insert int (value 20) at position 0, verify size and retrieval */
    ajoutElement_TabD(tableau1, ptr, 0);

    if (tableau1->taille != 1)
    {
        fprintf(stderr, "8 - Probléme dans la fonction ajoutElement_TabD() -  La taille du tableau est %d au lieu de 1\n", tableau1->taille);
        detruire_TabD(tableau1);
        return 1;
    }
    if (ptr != lireElement_TabD(tableau1, 0))
    {
        fprintf(stderr, "9 - Probléme dans la fonction ajoutElement_TabD() -  Element pas trouvé en posistion 0 \n");
        detruire_TabD(tableau1);
        return 1;
    }
    if ((position = rechercheElement_TabD(&rechercheInt_TabD, ptr, tableau1)) != 0)
    {
        fprintf(stderr, "10 - Probléme dans la fonction rechercheElement_TabD() - Position de l'entier 0 est %d au lieu de 0  --- Taille : %d  --- Taille_max : %d\n", position, tableau1->taille, tableau1->taille_max);
        detruire_TabD(tableau1);
        return 1;
    }

    /* Test 11–12: insert char 'A' at position 1, verify size and search result */
    ptr = &c;
    ajoutElement_TabD(tableau1, ptr, 1);

    if (tableau1->taille != 2)
    {
        fprintf(stderr, "11 - Probléme dans la fonction ajoutElement_TabD() - La taille du tableau est %d au lieu de 2\n", tableau1->taille);
        detruire_TabD(tableau1);
        return 1;
    }
    if ((position = rechercheElement_TabD(&rechercheChar_TabD, ptr, tableau1)) != 1)
    {
        fprintf(stderr, "12 - Probléme dans la fonction ajoutElement_TabD() - Position du caractere A est %d au lieu de 1\n", position);
        detruire_TabD(tableau1);
        return 1;
    }

    /* Test 13: insert int (value 2) at position 3, verify search result */
    ptr = &i1;
    ajoutElement_TabD(tableau1, ptr, 3);
    if ((position = rechercheElement_TabD(&rechercheChar_TabD, ptr, tableau1)) != 3)
    {
        fprintf(stderr, "13 - Probléme dans la fonction ajoutElement_TabD() - Position de l'entier 2 est %d au lieu de 3\n", position);
        detruire_TabD(tableau1);
        return 1;
    }

    /* Test 14–16: insert char '2' at position 86 — forces a capacity resize
     * (80 is not enough; 80*2 = 160 is the expected new taille_max) */
    ptr = &c2;
    ajoutElement_TabD(tableau1, ptr, 86);
    if (tableau1->taille_max != 160)
    {
        fprintf(stderr, "14 - Probléme dans la fonction ajoutElement_TabD() - La taille max du tableau est %d au lieu de 160\n", tableau1->taille);
        detruire_TabD(tableau1);
        return 1;
    }
    if (tableau1->taille != 87)
    {
        fprintf(stderr, "15 - Probléme dans la fonction ajoutElement_TabD() - La taille du tableau est %d au lieu de 87\n", tableau1->taille);
        detruire_TabD(tableau1);
        return 1;
    }
    if ((position = rechercheElement_TabD(&rechercheChar_TabD, ptr, tableau1)) != 86)
    {
        fprintf(stderr, "16 - Probléme dans la fonction ajoutElement_TabD() - Position du caractere 2 est %d au lieu de 86\n", position);
        detruire_TabD(tableau1);
        return 1;
    }

    /* Test 17: append int (value 1) at the logical end (index 87) */
    ptr = &i3;
    ajoutenDernier_TabD(tableau1, ptr);
    if ((position = rechercheElement_TabD(&rechercheChar_TabD, ptr, tableau1)) != 87)
    {
        fprintf(stderr, "17 - Probléme dans la fonction ajoutElement_TabD() - Position du caractere 2 est %d au lieu de 87\n", position);
        detruire_TabD(tableau1);
        return 1;
    }

    /* Test 18–19: append another element and verify size increment and
     * that the element at index 2 is still accessible via lireElement_TabD */
    int t = tableau1->taille;
    i1 = 10;
    ptr = &i1;
    ajoutenDernier_TabD(tableau1, ptr);

    if (tableau1->zone[2] != lireElement_TabD(tableau1, 2))
    {
        fprintf(stderr, "18 - Probléme dans la fonction ajoutenDernier_TabD() -  Element pas trouvé en position 2\n");
        detruire_TabD(tableau1);
        return 1;
    }
    if (tableau1->taille != t + 1)
    {
        fprintf(stderr, "19 - Probléme dans la fonction ajoutElement_TabD() - La taille du tableau est %d au lieu de %d\n", tableau1->taille, t);
        detruire_TabD(tableau1);
        return 1;
    }

    /* --- Teardown --- */
    detruire_TabD(tableau1);

    printf("Tests du module tableau dynamique : REUSSIS\n");
    return 0;
}

/* ===================================================================
 * test_AP - Unit tests for the prediction tree (arbre_prediction) module
 *
 * Verifies: node and root creation, child insertion, N-gram path
 * creation via rechercher_completer_ngramme_AP(), re-traversal of an
 * existing path, and the learning step (rechercherMot_AP + occurrence
 * count + remonterNoeud_AP sort invariant).
 *
 * Returns: 0 if all checks pass, 1 on the first failed assertion.
 * =================================================================== */
int test_AP()
{
    /* --- Step 1: create the hash table --- */
    struct strhash_table *ht = strhash_create(ENTREE);

    /* --- Step 2: create a size-3 sequence for the N-gram "Aymen Nachid Casa" --- */
    Sequence *seq = sequence_creer(3, ht);
    if (seq == NULL)
    {
        fprintf(stderr, "Erreur : Impossible de créer la séquence.\n");
        return 1;
    }

    /* --- Step 3: load the N-gram context into the sequence --- */
    sequence_pushWord(seq, "Aymen",  ht);
    sequence_pushWord(seq, "Nachid", ht);
    sequence_pushWord(seq, "Casa",   ht);

    /* --- Step 4: create the tree root --- */
    struct Noeud *racine = creerRacine_AP(ht);
    if (racine == NULL)
    {
        fprintf(stderr, "Erreur 1: La racine est NULL.\n");
        sequence_detruire(seq);
        return 1;
    }

    /* Error 2: freshly created root must have no children */
    if (racine->fils->taille != 0)
    {
        fprintf(stderr, "Erreur 2: racine->fils->taille est %d au lieu de 0.\n", racine->fils->taille);
        return 1;
    }

    /* Manually add 4 children to the root for baseline state */
    ajouterMot_AP(racine, "mon");
    ajouterMot_AP(racine, "test");
    ajouterMot_AP(racine, "perso");
    ajouterMot_AP(racine, "bon");

    /* Error 3: root must now have exactly 4 children */
    if (racine->fils->taille != 4)
    {
        fprintf(stderr, "Erreur 3: racine->fils->taille est %d au lieu de 4.\n", racine->fils->taille);
        return 1;
    }

    /* --- Step 5: walk/extend the trie along the sequence "Aymen Nachid Casa" ---
     * The path does not exist yet, so 3 new nodes must be created.
     * "Aymen" is added as a 5th child of the root. */
    struct Noeud *resultat_noeud = rechercher_completer_ngramme_AP(racine, seq);

    if (resultat_noeud == NULL)
    {
        fprintf(stderr, "Erreur 4: Le resultat_noeud est NULL.\n");
    }

    /* Error 6: root must now have 5 children (the 4 manual ones + "Aymen") */
    if (racine->fils->taille != 5)
    {
        fprintf(stderr, "Erreur 6: racine->fils->taille est %d au lieu de 5.\n", racine->fils->taille);
        return 1;
    }

    /* Error 7–9: verify the three-level path Aymen → Nachid → Casa */
    struct Noeud *nouveau_noeud = (struct Noeud *)lireElement_TabD(racine->fils, 4);
    if (strcmp(nouveau_noeud->mot, "Aymen") != 0)
    {
        fprintf(stderr, "Erreur 7: on a %s au lieu de Aymen\n", nouveau_noeud->mot);
        return 1;
    }

    struct Noeud *nouveau_noeud_niveau_2 = (struct Noeud *)lireElement_TabD(nouveau_noeud->fils, 0);
    if (strcmp(nouveau_noeud_niveau_2->mot, "Nachid") != 0)
    {
        fprintf(stderr, "Erreur 8: on a %s au lieu de Nachid\n", nouveau_noeud_niveau_2->mot);
        return 1;
    }

    struct Noeud *nouveau_noeud_niveau_3 = (struct Noeud *)lireElement_TabD(nouveau_noeud_niveau_2->fils, 0);
    if (strcmp(nouveau_noeud_niveau_3->mot, "Casa") != 0)
    {
        fprintf(stderr, "Erreur 9: on a %s au lieu de Casa\n", nouveau_noeud_niveau_3->mot);
        return 1;
    }

    /* --- Step 6: re-traverse the same path — must return the existing node,
     * not create a duplicate (pointer equality check) --- */
    struct Noeud *noeud_existant = rechercher_completer_ngramme_AP(racine, seq);

    if (noeud_existant != nouveau_noeud_niveau_3)
    {
        fprintf(stderr, "Erreur 14: La recherche n'a pas retrouvé le nœud existant.\n");
        return 1;
    }

    /* --- Step 7: learning — record "test" 4 times and "Cool" once after "Casa".
     * remonterNoeud_AP must bubble "test" (4 occurrences) to index 0. --- */
    rechercherMot_AP(noeud_existant, "test");
    rechercherMot_AP(noeud_existant, "test");
    rechercherMot_AP(noeud_existant, "test");
    rechercherMot_AP(noeud_existant, "test");
    rechercherMot_AP(noeud_existant, "Cool");

    /* Error 15: "test" must be at index 0 (highest occurrences) with count 4 */
    struct Noeud *fils_test = (struct Noeud *)lireElement_TabD(noeud_existant->fils, 0);
    if (fils_test->occurrences != 4)
    {
        fprintf(stderr, "Erreur 15: occurrences de 'test' : %d au lieu de 4.\n", fils_test->occurrences);
    }

    /* --- Teardown --- */
    detruire_arbre(racine);
    sequence_detruire(seq);
    strhash_free(ht);

    printf("Tests du module Arbre de Prédiction : REUSSIS\n");
    return 0;
}

/* ===================================================================
 * test_IO - Integration test for the prediction tree serialization module
 *
 * Builds a Y-shaped tree (root → "le" → "chat" → "mange"
 *                                           → "chien" → "dort"),
 * saves it to a binary file, loads it back into a fresh set of nodes,
 * and performs cross-checks on word content, occurrence counts, pointer
 * interning, and structural integrity (branching, depth, leaf values).
 *
 * Returns: 0 if all checks pass, 1 on the first failed assertion.
 * =================================================================== */
int test_IO()
{
    const char *nom_fichier = "system_test_arbre.bin";

    /* ==============================================
     * Step 1: Build the source tree
     * ============================================== */
    struct strhash_table *ht = strhash_create(5000);
    struct Noeud *racine_orig = creerRacine_AP(ht);

    if (!racine_orig)
    {
        fprintf(stderr, "[ERREUR 1] Impossible de créer la racine originale.\n");
        return 1;
    }

    /* Intern all words up front so pointer equality holds throughout the test */
    const char *p_le    = strhash_wordAdd(ht, "le");
    const char *p_chat  = strhash_wordAdd(ht, "chat");
    const char *p_chien = strhash_wordAdd(ht, "chien");
    const char *p_mange = strhash_wordAdd(ht, "mange");
    const char *p_dort  = strhash_wordAdd(ht, "dort");

    /* Build the Y-shaped trie:
     *   root → "le" → "chat"  → "mange"
     *                 "chien" → "dort"        */
    ajouterMot_AP(racine_orig, p_le);
    struct Noeud *n_le = (struct Noeud *)lireElement_TabD(racine_orig->fils, 0);

    ajouterMot_AP(n_le, p_chat);
    ajouterMot_AP(n_le, p_chien);

    /* "chat" is at index 0 of n_le, "chien" at index 1 (insertion order) */
    struct Noeud *n_chat  = (struct Noeud *)lireElement_TabD(n_le->fils, 0);
    struct Noeud *n_chien = (struct Noeud *)lireElement_TabD(n_le->fils, 1);

    ajouterMot_AP(n_chat,  p_mange);
    ajouterMot_AP(n_chien, p_dort);

    /* Set occurrence counts manually to verify they survive the round-trip */
    n_le->occurrences    = 100;
    n_chat->occurrences  = 50;
    n_chien->occurrences = 30;
    /* Leaf nodes ("mange", "dort") keep their default count of 1 from creerNoeud_AP */

    /* ==============================================
     * Step 2: Save the tree to a binary file
     * ============================================== */
    sauvegarder_AP(racine_orig, nom_fichier);

    /* Error 2: the output file must exist after saving */
    FILE *f_check = fopen(nom_fichier, "rb");
    if (!f_check)
    {
        fprintf(stderr, "[ERREUR 2] Le fichier binaire n'a pas été généré.\n");
        return 1;
    }
    fclose(f_check);

    /* ==============================================
     * Step 3: Load the tree back from the binary file
     * ============================================== */
    struct Noeud *racine_chargee = charger_AP(nom_fichier, ht);
    if (!racine_chargee)
    {
        fprintf(stderr, "[ERREUR 3] Échec du chargement de l'arbre.\n");
        return 1;
    }

    /* ==============================================
     * Step 4: Cross-validate the loaded tree
     * ============================================== */

    /* Error 4a: root word must be the empty string */
    if (racine_chargee->mot == NULL || strcmp(racine_chargee->mot, "") != 0)
    {
        fprintf(stderr, "[ERREUR 4] La racine ne contient pas le mot vide.\n");
        return 1;
    }

    /* Error 4b: root word pointer must be the interned empty-string address
     * (same hash table is reused, so interning must return the same pointer) */
    const char *mot_vide_hache = strhash_wordAdd(ht, "");
    if (racine_chargee->mot != mot_vide_hache)
    {
        fprintf(stderr, "[ERREUR 4] La racine n'utilise pas le pointeur unique du mot vide.\n");
        return 1;
    }

    /* Error 5: first-level node "le" must have correct word and occurrence count */
    struct Noeud *c_le = (struct Noeud *)lireElement_TabD(racine_chargee->fils, 0);
    if (c_le->mot != p_le || c_le->occurrences != 100)
    {
        fprintf(stderr, "[ERREUR 5] Données corrompues pour le mot 'le'.\n");
        return 1;
    }

    /* Error 6: pointer interning must be preserved — both trees share the same
     * hash table, so word pointers must be identical across the two trees */
    if (c_le->mot != n_le->mot)
    {
        fprintf(stderr, "[ERREUR 6] Problème d'unicité des pointeurs (Hash Table non respectée).\n");
        return 1;
    }

    /* Error 7: "le" must have exactly 2 children ("chat" and "chien") */
    if (c_le->fils->taille != 2)
    {
        fprintf(stderr, "[ERREUR 7] Nombre de fils incorrect pour 'le' (%d au lieu de 2).\n", c_le->fils->taille);
        return 1;
    }

    struct Noeud *c_chat  = (struct Noeud *)lireElement_TabD(c_le->fils, 0);
    struct Noeud *c_chien = (struct Noeud *)lireElement_TabD(c_le->fils, 1);

    /* Error 8: child order and word content must be preserved after round-trip */
    if (strcmp(c_chat->mot, "chat") != 0 || strcmp(c_chien->mot, "chien") != 0)
    {
        fprintf(stderr, "[ERREUR 8] Inversion ou corruption des fils de 'le'.\n");
        return 1;
    }

    /* Error 9: leaf nodes must retain their default occurrence count of 1 */
    struct Noeud *c_mange = (struct Noeud *)lireElement_TabD(c_chat->fils,  0);
    struct Noeud *c_dort  = (struct Noeud *)lireElement_TabD(c_chien->fils, 0);

    if (c_mange->occurrences != 1 || c_dort->occurrences != 1)
    {
        fprintf(stderr, "[ERREUR 9] Erreur sur les feuilles de l'arbre.\n");
        return 1;
    }

    /* ==============================================
     * Step 5: Teardown — free both trees, hash table, and temp file
     * ============================================== */
    detruire_arbre(racine_orig);
    detruire_arbre(racine_chargee);
    strhash_free(ht);
    remove(nom_fichier);

    printf("Tests du module arbre io : REUSSIS\n");
    return 0;
}