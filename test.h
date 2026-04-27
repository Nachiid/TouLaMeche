/************************************************************************
  Nom du fichier : test.c
=============================================================
  Description : Test les fonctions du programme
=============================================================
  Auteur : Nachid Ayman
=============================================================
************************************************************/

#ifndef _TEST_H_
#define _TEST_H_

int test_sequence();
int test_TabD();
int test_AP();
int test_IO();

int rechercheInt_TabD(const void *element_recherche, const void *element_TabD);
int rechercheChar_TabD(const void *element_recherche, const void *element_TabD);
void AfficherElementInt_TabD(const void *element);
void AfficherElementChar_TabD(const void *element);

#endif
