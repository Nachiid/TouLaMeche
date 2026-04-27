/************************************************************************
 * File    : sequence.h
 * =============================================================
 * Description : Public interface for the sequence module.
 *               A Sequence is a fixed-size circular buffer of
 *               interned string pointers representing an N-gram
 *               window over a stream of words.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#ifndef _ARBRE_IO_H
#define _ARBRE_IO_H

#include "list.h"
#include "hash.h"
#include <stdio.h>

void sauvegarder_AP(struct Noeud *racine, const char *nom_fichier);
void sauvegarder_rec(struct Noeud *n, FILE *f);
struct Noeud* charger_AP(const char* nom_fichier, struct strhash_table *ht);
void normaliser_mot(char *str);


#endif /* _ARBRE_IO_H */