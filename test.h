/************************************************************************
 * File    : test.h
 * =============================================================
 * Description : Public interface for the test module.
 *               Declares unit and integration test functions for all
 *               project modules, as well as the local comparison and
 *               display callbacks used by the dynamic array tests.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#ifndef _TEST_H_
#define _TEST_H_

/* -------------------------------------------------------------------
 * Test functions
 *
 * Each function is self-contained: it sets up its own data structures,
 * runs a series of assertions, tears down all allocated resources, and
 * prints a success message to stdout if all checks pass.
 *
 * Returns: 0 on success, 1 on the first failed assertion.
 * ------------------------------------------------------------------- */

/* test_sequence - Unit tests for the Sequence circular buffer module */
int test_sequence();

/* test_TabD - Unit tests for the dynamic array module */
int test_TabD();

/* test_AP - Unit tests for the prediction tree module */
int test_AP();

/* test_IO - Integration test for the tree serialization/deserialization module */
int test_IO();

int test_apprentissage();

/* -------------------------------------------------------------------
 * Dynamic array callbacks (defined in test.c, used by test_TabD)
 *
 * These are local predicates and display functions compatible with the
 * void* callback signatures of rechercheElement_TabD() and
 * AfficherElement_Tabd(). They are declared here so that other test
 * files can reuse them without duplicating the implementations.
 * ------------------------------------------------------------------- */

/*
 * rechercheInt_TabD - Predicate: match two int* elements by value
 * @element_recherche: void* of the reference int
 * @element_TabD:      void* of the stored int
 *
 * Returns: 1 if both integers are equal, 0 otherwise.
 */
int rechercheInt_TabD(const void *element_recherche, const void *element_TabD);

/*
 * rechercheChar_TabD - Predicate: match two char* elements by string content
 * @element_recherche: void* of the reference string
 * @element_TabD:      void* of the stored string
 *
 * Returns: 1 if both strings are equal (strcmp == 0), 0 otherwise.
 */
int rechercheChar_TabD(const void *element_recherche, const void *element_TabD);

/*
 * AfficherElementInt_TabD - Print a stored element cast to int*
 * @element: void* of an int to dereference and print
 */
void AfficherElementInt_TabD(const void *element);

/*
 * AfficherElementChar_TabD - Print a stored element cast to char*
 * @element: void* of a char to dereference and print
 */
void AfficherElementChar_TabD(const void *element);

#endif /* _TEST_H_ */