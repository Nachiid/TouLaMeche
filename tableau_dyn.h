/************************************************************************
  Nom du fichier : tableau_dyn.h
=============================================================
  Description : Contient les fonctions du module tableau dynamique
=============================================================
  Auteur : Nachid Ayman
=============================================================
************************************************************************/

#ifndef TABLEAU_DYN_H
#define TABLEAU_DYN_H

// Tableau dynamique générique à base de pointeurs void*
struct table_D
{
    void **zone;    // Zone mémoire contenant les éléments
    int taille;     // Nombre d'éléments stockés
    int taille_max; // Capacité actuelle du tableau
    int facteur;    // Facteur de croissance lors du redimensionnement
};

/* --- Gestion de la structure --- */

// Alloue et initialise un tableau dynamique avec une taille et un facteur de croissance
struct table_D *creer_TabD(int taille_max, int facteur);

// Libère la mémoire du tableau dynamique
void detruire_TabD(struct table_D *t);

/* --- Modification --- */

// Agrandit la zone mémoire jusqu'à atteindre la taille minimale requise
void majtaille_TabD(struct table_D *t, int min);

// Insère un élément à la position donnée, en décalant les suivants si nécessaire
void ajoutElement_TabD(struct table_D *t, void *element, int position);

// Ajoute un élément en dernière position
void ajoutenDernier_TabD(struct table_D *t, void *element);

// Écrit un élément à la position donnée, ou l'ajoute si hors de la taille actuelle
void ecrireElement_TabD(struct table_D *t, void *element, int position);

/* --- Accès --- */

// Retourne l'élément à la position donnée, ou NULL si invalide
void *lireElement_TabD(const struct table_D *t, int position);

// Retourne l'index du premier élément correspondant, ou -1 si non trouvé
int rechercheElement_TabD(int (*pf)(const void *, const void *), const void *element, const struct table_D *t);

/* --- Affichage --- */

// Affiche l'élément à la position donnée via une fonction de rappel
void AfficherElement_Tabd(void (*pf)(const void *), const struct table_D *t, int position);

// Affiche un élément de type char
void AfficherElementChar_TabD(const void *element);

// Affiche un élément de type int
void AfficherElementInt_TabD(const void *element);

#endif // TABLEAU_DYN_H