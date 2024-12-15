/************************************************************************
  Nom du fichier : tableau_dyn.h
  Description : Déclaration du module pour un tableau dynamique.
  Auteur : Nachid Ayman
************************************************************************/

#ifndef TABLEAU_DYN_H
#define TABLEAU_DYN_H

// Structure représentant un tableau dynamique
// Elle contient un pointeur vers la zone de stockage des éléments,
// la taille actuelle du tableau, la taille maximale, et un facteur de croissance.
struct table_D
{
    void **zone;       // Pointeur vers la zone mémoire contenant les éléments du tableau
    int taille;        // Taille actuelle du tableau (nombre d'éléments stockés)
    int taille_max;    // Taille maximale du tableau (capacité actuelle)
    int facteur;       // Facteur de croissance pour redimensionner le tableau
};

// Déclaration de la fonction pour créer un tableau dynamique avec une taille maximale et un facteur de croissance
// Cette fonction alloue la mémoire pour la structure de tableau dynamique et initialise ses paramètres.
struct table_D *creer_TabD(int taille_max, int facteur);

// Déclaration de la fonction pour détruire un tableau dynamique et libérer la mémoire associée
// Cette fonction libère la mémoire allouée pour la structure et la zone de stockage du tableau.
void detruire_TabD(struct table_D *t);

// Déclaration de la fonction pour redimensionner le tableau dynamique (ajuster la taille maximale)
// Cette fonction agrandit la zone mémoire du tableau si nécessaire, en utilisant le facteur de croissance.
void majtaille_TabD(struct table_D *t, int min);

// Déclarations des fonctions pour ajouter des éléments au tableau dynamique

// Ajoute un élément à une position spécifique dans le tableau dynamique.
// Si la position est en dehors de la taille actuelle, le tableau est redimensionné.
void ajoutElement_TabD(struct table_D *t, void *element, int position);

// Ajoute un élément à la dernière position disponible dans le tableau dynamique.
void ajoutenDernier_TabD(struct table_D *t, void *element);

// Déclarations des fonctions pour lire et écrire des éléments dans le tableau dynamique

// Écrit un élément à une position spécifique dans le tableau dynamique.
// Si la position est en dehors de la taille actuelle, l'élément est ajouté à cette position.
void ecrireElement_TabD(struct table_D *t, void *element, int position);

// Lit un élément à une position spécifique dans le tableau dynamique.
// Retourne un pointeur vers l'élément ou NULL si la position est hors limites.
void *lireElement_TabD(struct table_D *t, int position);

// Déclaration de la fonction pour rechercher un élément dans le tableau dynamique
// Cette fonction parcourt le tableau et retourne l'index de l'élément trouvé en utilisant une fonction de comparaison.
// Si l'élément n'est pas trouvé, elle retourne -1.
int rechercheElement_TabD(int (*pf)(const void *, const void *), void *element, struct table_D *t);

// Déclarations des fonctions pour afficher les éléments du tableau dynamique

// Affiche l'élément à une position spécifique du tableau.
// Cette fonction utilise un pointeur vers une fonction d'affichage pour afficher l'élément de manière générique.
void AfficherElement_Tabd(void (*pf)(const void *), struct table_D *t, int position);

// Fonction d'affichage spécifique pour un élément de type 'char'
void AfficherElementChar_TabD(const void *element);

// Fonction d'affichage spécifique pour un élément de type 'int'
void AfficherElementInt_TabD(const void *element);

#endif // TABLEAU_DYN_H
