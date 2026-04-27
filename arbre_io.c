#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "error.h"
#include "sequence.h"
#include "tableau_dyn.h"
#include "arbre_prediction.h"

// Supposons que ton module TabD a une fonction pour obtenir la taille :
// int taille_TabD(struct table_D *t);
// Et une pour obtenir un élément :
// struct Noeud* element_TabD(struct table_D *t, int index);

void sauvegarder_rec(struct Noeud *n, FILE *f)
{
    if (n == NULL || f == NULL)
        return;

    // 1. Calcul et écriture de la taille du mot
    int taille = 0;
    if (n->mot != NULL)
    {
        // n->mot provient de struct Noeud
        taille = strlen(n->mot) + 1;
    }
    fwrite(&taille, sizeof(int), 1, f);

    // 2. Écriture du texte
    if (taille > 0)
    {
        fwrite(n->mot, sizeof(char), taille, f);
    }

    // 3. Écriture des occurrences (Champ de struct Noeud)
    fwrite(&(n->occurrences), sizeof(int), 1, f);

    // 4. Écrire le nombre de fils
    // On accède directement au champ 'taille' de struct table_D
    int nb_fils = n->fils->taille;
    fwrite(&nb_fils, sizeof(int), 1, f);

    // 5. Boucle sur chaque fils
    for (int i = 0; i < nb_fils; i++)
    {
        // Utilisation de lireElement_TabD définie dans ton module
        // On cast le void* en struct Noeud*
        struct Noeud *enfant = (struct Noeud *)lireElement_TabD(n->fils, i);

        // Appel récursif avec le bon nom de fonction
        sauvegarder_rec(enfant, f);
    }
}

//
void sauvegarder_AP(struct Noeud *racine, const char *nom_fichier)
{
    FILE *f = fopen(nom_fichier, "wb");
    if (f == NULL || racine == NULL)
        return;

    // 1. Écrire une taille factice (0) pour simuler le mot NULL de la racine
    int taille_ignoree = 0;
    fwrite(&taille_ignoree, sizeof(int), 1, f);

    // 2. Écrire les occurrences de la racine
    fwrite(&(racine->occurrences), sizeof(int), 1, f);

    // 3. Écrire le nombre de mots du dictionnaire (les fils de la racine)
    int nb_mots_dictionnaire = racine->fils->taille;
    fwrite(&nb_mots_dictionnaire, sizeof(int), 1, f);

    // 4. Lancer la récursion normale sur chaque mot du dictionnaire
    for (int i = 0; i < nb_mots_dictionnaire; i++)
    {
        struct Noeud *enfant = (struct Noeud *)lireElement_TabD(racine->fils, i);
        sauvegarder_rec(enfant, f); // Ta fonction récursive
    }

    fclose(f);
}

struct Noeud *charger_noeud_recursif(FILE *f, struct strhash_table *ht)
{
    int taille_mot;
    if (fread(&taille_mot, sizeof(int), 1, f) != 1)
        return NULL;

    const char *mot_final = NULL;
    if (taille_mot > 0)
    {
        char *temp = malloc(taille_mot);
        fread(temp, sizeof(char), taille_mot, f);
        // On récupère le pointeur unique dans ta table de hachage
        mot_final = strhash_wordAdd(ht, temp);
        free(temp);
    }

    // Création du nœud normal avec ta fonction
    struct Noeud *n = creerNoeud_AP(mot_final);

    fread(&(n->occurrences), sizeof(int), 1, f);

    int nb_fils;
    fread(&nb_fils, sizeof(int), 1, f);

    for (int i = 0; i < nb_fils; i++)
    {
        struct Noeud *enfant = charger_noeud_recursif(f, ht);
        if (enfant != NULL)
        {
            ajoutenDernier_TabD(n->fils, enfant); //
        }
    }

    return n;
}

struct Noeud *charger_AP(const char *nom_fichier, struct strhash_table *ht)
{
    FILE *f = fopen(nom_fichier, "rb");
    if (f == NULL)
        return NULL;

    // 1. On crée la racine vide (le mot NULL) avec ta fonction dédiée
    struct Noeud *racine = creerRacine_AP(ht); // Défini dans arbre_prediction.h

    // 2. On lit les métadonnées de la racine dans le fichier
    // On ignore la taille et le mot (car la racine est toujours NULL/vide)
    int taille_ignoree;
    fread(&taille_ignoree, sizeof(int), 1, f);

    // On lit les occurrences de la racine et son nombre de fils
    fread(&(racine->occurrences), sizeof(int), 1, f);

    int nb_mots_dictionnaire;
    fread(&nb_mots_dictionnaire, sizeof(int), 1, f);

    // 3. On charge tous les fils de la racine (tes 100 000 mots)
    for (int i = 0; i < nb_mots_dictionnaire; i++)
    {
        struct Noeud *enfant = charger_noeud_recursif(f, ht);
        if (enfant != NULL)
        {
            // On utilise le module de tableau dynamique pour lier l'enfant à la racine
            ajoutenDernier_TabD(racine->fils, enfant); // Défini dans tableau_dyn.h
        }
    }

    fclose(f);
    return racine;
}

void normaliser_mot(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower((unsigned char)str[i]);
    }
}