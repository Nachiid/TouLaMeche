/* --------------------------------------------------------------------
   module hash table
   --------------------------------------------------------------------
   22/10/2015	SR	commentaires
   --------------------------------------------------------------------
*/
#ifndef _STRHASH_H_
#define _STRHASH_H_

#include "list.h"

#define ENTREE 10000 // Taille de la table de hachage pour l'exemple

struct strhash_table
{
	unsigned int size;
	struct keyList
	{
		s_node *first;
		int nbElements;
	} *values;
};

struct strhash_stat
{
	unsigned int nbWord;
	unsigned int nbWordMin, nbWordMax;
	double nbWordMean, stdDev;
};

// creation d'une table de hashage de size entrees
struct strhash_table *strhash_create(int size);

// effacement de toutes les entrees de la tables
//  avec liberation des zones memoires allouees aux mots
void strhash_reset(struct strhash_table *ht);

// effacement et destruction de la table
void strhash_free(struct strhash_table *ht);

// ajout d'un mot dans la table de hachage si celui-ci ne s'y trouve pas deja
char *strhash_wordAdd(struct strhash_table *ht, const char *word);

// suppression d'un mot dans la table de hachage
int strhash_wordDel(struct strhash_table *ht, const char *word);

// affichage des mots enregistres dans la table
void strhash_display(struct strhash_table *ht);

// statistiques sur l'occupation de la table
//   les valeurs sont retournes dans la struct stat
void strhash_analyse(const struct strhash_table *ht, struct strhash_stat *stat);

#endif /*_STRHASH_H_*/
