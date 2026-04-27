/************************************************************************
  Nom du fichier : arbre_prediction.c
=============================================================
  Description : Contient les fonctions du module d'arbre de prédiction
                avec gestion centralisée des erreurs.
=============================================================
  Auteur : Nachid Ayman
================================================*************/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "sequence.h"
#include "tableau_dyn.h"
#include "arbre_prediction.h"

/**
 * creerNoeud_AP - Alloue et initialise un nouveau nœud
 * @mot: Pointeur vers le mot (géré par la table de hachage)
 */
struct Noeud *creerNoeud_AP(const char *mot)
{
    if (mot == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Tentative de creer un noeud avec un mot NULL");
        return NULL;
    }

    struct Noeud *mon_noeud = (struct Noeud *)malloc(sizeof(struct Noeud));
    if (mon_noeud == NULL)
    {
        error_print(ERR_ALLOC, "TREE", "Echec malloc structure Noeud");
        return NULL;
    }

    mon_noeud->mot = mot;
    mon_noeud->occurrences = 1;
    
    // Initialisation du tableau des fils
    mon_noeud->fils = creer_TabD(2, 2);
    if (mon_noeud->fils == NULL)
    {
        // error_print est déjà géré dans creer_TabD, on nettoie juste
        free(mon_noeud);
        return NULL;
    }

    return mon_noeud;
}

/**
 * creerRacine_AP - Crée la racine de l'arbre (mot vide)
 */
struct Noeud *creerRacine_AP(struct strhash_table *ht)
{
    struct Noeud *racine = (struct Noeud *)malloc(sizeof(struct Noeud));
    if (racine == NULL)
    {
        error_print(ERR_ALLOC, "TREE", "Echec malloc racine");
        return NULL;
    }

    racine->mot = strhash_wordAdd(ht, "");
    racine->occurrences = 0;
    racine->fils = creer_TabD(5, 2);
    
    if (racine->fils == NULL)
    {
        free(racine);
        return NULL;
    }

    return racine;
}

/**
 * rechercher_completer_ngramme_AP - Navigue dans l'arbre selon une séquence
 * et crée les nœuds manquants si nécessaire.
 */
struct Noeud *rechercher_completer_ngramme_AP(struct Noeud *racine, Sequence *seq)
{
    if (racine == NULL || seq == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Racine ou Sequence NULL dans la recherche de n-gramme");
        return NULL;
    }

    struct Noeud *noeud_courant = racine;
    sequence_itStart(seq);

    while (sequence_itHasNext(seq))
    {
        const char *mot = sequence_itNext(seq);
        int index = rechercheElement_TabD(comparerMot_AP, mot, noeud_courant->fils);

        if (index != -1)
        {
            noeud_courant = (struct Noeud *)lireElement_TabD(noeud_courant->fils, index);
        }
        else
        {
            // Le chemin n'existe pas, on le crée
            ajouterMot_AP(noeud_courant, mot);
            // On se déplace sur le dernier élément ajouté
            int dernier = noeud_courant->fils->taille - 1;
            noeud_courant = (struct Noeud *)lireElement_TabD(noeud_courant->fils, dernier);
        }
    }

    return noeud_courant;
}

/**
 * ajouterMot_AP - Ajoute un nouveau mot comme fils d'un nœud
 */
void ajouterMot_AP(struct Noeud *mon_noeud, const char *mot)
{
    if (mon_noeud == NULL || mot == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Argument NULL dans ajouterMot_AP");
        return;
    }

    struct Noeud *nouveau = creerNoeud_AP(mot);
    if (nouveau != NULL)
    {
        ajoutenDernier_TabD(mon_noeud->fils, nouveau);

    }
}

/**
 * rechercherMot_AP - Apprentissage : incrémente ou ajoute un mot suivant
 */
void rechercherMot_AP(struct Noeud *noeud_final, const char *mot)
{
    if (noeud_final == NULL || mot == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "Argument NULL dans rechercherMot_AP");
        return;
    }

    int index = rechercheElement_TabD(comparerMot_AP, mot, noeud_final->fils);

    if (index != -1)
    {
        struct Noeud *cible = (struct Noeud *)lireElement_TabD(noeud_final->fils, index);
        cible->occurrences++;
        // On maintient l'ordre décroissant pour la prédiction en O(1)
        remonterNoeud_AP(noeud_final->fils, index);
    }
    else
    {
        ajouterMot_AP(noeud_final, mot);
    }
}

/**
 * MotMaxOccurrences_AP - Retourne la prédiction la plus probable
 */
struct Noeud *MotMaxOccurrences_AP(struct Noeud *noeud_courant)
{
    if (noeud_courant == NULL)
    {
        ERROR_DEBUG(ERR_NULL_POINTER, "noeud_courant NULL dans MotMaxOccurrences_AP");
        return NULL;
    }

    if (noeud_courant->fils == NULL || noeud_courant->fils->taille == 0)
    {
        return NULL;
    }

    // Grâce à remonterNoeud_AP, le maximum est toujours à l'index 0
    return (struct Noeud *)lireElement_TabD(noeud_courant->fils, 0);
}

int comparerMot_AP(const void *noeud_v, const void *mot_v)
{
    const struct Noeud *noeud = (const struct Noeud *)noeud_v;
    const char *mot_cherche = (const char *)mot_v;
    
    // Comparaison d'adresses car les mots viennent de la table de hachage
    return (noeud->mot == mot_cherche);
}

void detruire_arbre(struct Noeud *noeud)
{
    if (noeud == NULL)
    {
        return; // free(NULL) est valide et ne provoque pas d'erreur
    }

    if (noeud->fils != NULL)
    {
        for (int i = 0; i < noeud->fils->taille; i++)
        {
            struct Noeud *fils_courant = (struct Noeud *)lireElement_TabD(noeud->fils, i);
            detruire_arbre(fils_courant);
        }
        detruire_TabD(noeud->fils);
    }

    /* * NOTE: We do NOT free(noeud->mot) because the strings are owned 
     * by the Hash Table, not the tree nodes :)
     */
    noeud->mot = NULL;
    free(noeud);
}

/**
 * remonterNoeud_AP - Maintient le tableau des fils trié par occurrences (décroissant)
 */
void remonterNoeud_AP(struct table_D *fils, int index)
{
    if (fils == NULL) return;

    while (index > 0)
    {
        struct Noeud *courant = (struct Noeud *)lireElement_TabD(fils, index);
        struct Noeud *precedent = (struct Noeud *)lireElement_TabD(fils, index - 1);

        if (courant->occurrences > precedent->occurrences)
        {
            // Cette fonction doit être présente dans ton module tableau_dyn
            echangerElements_TabD(fils, index, index - 1);
            index--;
        }
        else
        {
            break; 
        }
    }
}

void afficherMot_AP(const void *noeud_v)
{
    const struct Noeud *n = (const struct Noeud *)noeud_v;
    if (n != NULL && n->mot != NULL)
    {
        printf("%s (%d)\n", n->mot, n->occurrences);
    }
}

void afficherArbre(const struct Noeud *racine, int hauteur)
{
    if (racine == NULL) return;

    for (int i = 0; i < hauteur; i++) printf("  ");
    afficherMot_AP(racine);

    if (racine->fils != NULL)
    {
        for (int i = 0; i < racine->fils->taille; i++)
        {
            struct Noeud *f = (struct Noeud *)lireElement_TabD(racine->fils, i);
            afficherArbre(f, hauteur + 1);
        }
    }
}