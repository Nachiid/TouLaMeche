/* -------------------------------------------------------------
   Test module hash
   -------------------------------------------------------------
   15/10/18 	SR	hash_wordDel --> strhash_wordDel
			cast (const char *) vers (char *) dans wordDel
   -------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"


int main() {
	struct strhash_stat hstat;
	struct strhash_table * ht;
	const char * mot1="hash", *mot2="la";

	int i;
	

	if ( ( ht=strhash_create(1000))==NULL ) {
		fprintf(stderr, "10 - strhash_create\n ");
	}
	
	strhash_analyse(ht, &hstat);
	if (hstat.nbWord!=0U) {
		fprintf(stderr, "20 - table non vide (%d mots)\n", hstat.nbWord);
	}
	
	if ( strcmp(strhash_wordAdd(ht, mot1),mot1)) {
		fprintf(stderr, "30 - ajout incorrect\n");
	}
	
	if ( strcmp(strhash_wordAdd(ht, mot1),mot1)) {
		fprintf(stderr, "35 - ajout incorrect (mot en double)\n");
	}
	
	strhash_analyse(ht, &hstat);
	if (hstat.nbWord!=1U) {
		fprintf(stderr, "40 -  %d mots au lieu de 1\n", hstat.nbWord);
	}
		
	if ( strcmp(strhash_wordAdd(ht, mot2), mot2)) {
		fprintf(stderr, "50 - ajout incorrect \n");
	}
		
	strhash_analyse(ht, &hstat);
	if (hstat.nbWord!=2U) {
		fprintf(stderr, "60 -  %d mots au lieu de 2\n", hstat.nbWord);
	}
	
	printf("Affichage ateendu :\nla\nhash\n");
	strhash_display(ht);   // affiche   "la"\n"hash"
	
	if ( strhash_wordDel(ht, mot1) )
		fprintf(stderr, "70 - sup incorrect \n");
	
	strhash_analyse(ht, &hstat);
	if (hstat.nbWord!=1U) {
		fprintf(stderr, "80 -  %d mots au lieu de 1\n", hstat.nbWord);
	}
	
	// test intensif
	srand(10);
	for(i=0;i<999999; i++) {
		char *word;
		int size=rand()%32+10; // 31 lettres max
		int l;
		word=(char *)malloc(size*sizeof(char));
		for(l=0; l<size-1;l++) {
			word[l]=rand()%(128-' ')+' ';
		}
		word[l]='\0';
		if ( strcmp(strhash_wordAdd(ht, word), word)) 
				fprintf(stderr, "95 - erreur d'ajout mot %d\n",i);
		free(word);
		
	}
	strhash_analyse(ht, &hstat);
	fprintf(stdout, "Stat apres test d'insertion intensif (1000000 de mots)\n");
	fprintf(stdout, "Nb mots %u\nMoyenne par clef %f\nMin %u max %u\n",
			hstat.nbWord, hstat.nbWordMean, hstat.nbWordMin, hstat.nbWordMax);
	fprintf(stdout, "Ecart type %f\n", hstat.stdDev);	
	strhash_reset(ht);
	strhash_analyse(ht, &hstat);
	if (hstat.nbWord!=0U) {
		fprintf(stderr, "90-  RAZ non reussi (reste %d mots)\n", hstat.nbWord);
	}
	
	if ( strcmp(strhash_wordAdd(ht, mot1),mot1)) {
		fprintf(stderr, "100 - ajout incorrect \n");
	}
	
	strhash_analyse(ht, &hstat);
	if (hstat.nbWord!=1U) {
		fprintf(stderr, "110 -  %d mots au lieu de 1\n", hstat.nbWord);
	}
	
	strhash_free(ht);
	fprintf(stderr, "fin du test\n");
	
	return 0;	
}
