/* --------------------------------------------------------------------
   module entree_sorie.h
   --------------------------------------------------------------------
   22/10/2015	SR	commentaires
   --------------------------------------------------------------------
*/

#define stdin (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

#include "hash.h"        // Inclusion du fichier de la table de hachage
#include "list.h"        // Inclusion du fichier de la liste
#include "sequence.h"    // Inclusion du fichier de la séquence de mots
#include "tableau_dyn.h" // Inclusion du fichier de gestion du tableau dynamique
#include "arbre_prediction.h"

#ifndef _ENTREE_SORTIE_H
#define _ENTREE_SORTIE_H

extern struct strhash_table *DICTIONNAIRE; // TABLE HT AVEC LES MOTS DU DICTIONNAIRE
extern struct Noeud *ARBRE;                // ARBRE CONTENANT DES SEQUENCES DE MOT DE HT

extern struct _iobuf
{
    int _cnt;
    unsigned char *_ptr;
    unsigned char *_base;
    int _bufsiz;
    short _flag;
    char _file;
} _iob[];

#define FILE struct _iobuf
#define EOF (-1)

int saisieTexte(const char *fichier_texte);
int Ajout_texte(const char *fichier_texte);

#endif /*_ENTREE_SORTIE_H*/
