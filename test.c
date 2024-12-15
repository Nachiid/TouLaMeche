/************************************************************************
  Nom du fichier : test.c
=============================================================
  Description : Test les fonctions du programme
=============================================================
  Auteur : Nachid Ayman
=============================================================
************************************************************/
// #define NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "list.h"
#include "sequence.h"
#include "tableau_dyn.h"
#include "arbre_prediction.h"

// Test du module séquence
int test_sequence()
{

  // Création d'une table de hachage de taille ENTREE
  struct strhash_table *ht = strhash_create(ENTREE);
  struct strhash_stat hstat;
  const char *dernierMot = NULL;

  // Initialisation de la sequence
  sequence_initialise(ht);
  // Test de la fonction sequence_initialise()
  if (ht == NULL)
  {
    fprintf(stderr, "1 - Erreur lors de la création de la table de hachage.\n");
    strhash_free(ht);
    return 1;
  }

  // Test de nombre de mot aprés initialisation
  strhash_analyse(ht, &hstat);
  if (hstat.nbWord != 1U)
  {
    fprintf(stderr, "2 - Erreur dans sequence_initialise  - Mot dans la table au lieu de 1 : %d\n", hstat.nbWord);
    strhash_free(ht);
    return 1;
  }

  // Test que le dernier mot est le mot vide
  dernierMot = sequence_nextWord();
  if (strcmp(dernierMot, ""))
  {
    fprintf(stderr, "3 - Erreur dans l'ajout de mot vide  - Mot dans la table au lieu de "
                    ": %s\n",
            dernierMot);
    strhash_free(ht);
    return 1;
  }

  // Verification que exactement 4 mots on été ajouté dans ht et 3 derniers dans le N_gramme sont TEST1 TEST2 TEST3
  char wordi[256];
  for (int i = 0; i < 4; i++)
  {
    sprintf(wordi, "TEST%d", i);
    sequence_addWord(wordi, ht);
    sequence_progress();
  }
  // Test des 3 derniers mot de la sequence
  if (!strcmp(sequence_printInTab(), "TEST1 TEST2 TEST3"))
  {
    fprintf(stderr, "4 - Erreur dans sequence_itStart.\nRésultat attendu : TEST1 TEST2 TEST3\nRésultat affiché : %s\n", sequence_printInTab());
    strhash_free(ht);
    return 1;
  }

  // Affiche le contenue de la table de hashge
  // strhash_display(ht);

  // Renitialisation de ht pour un test intensif
  strhash_reset(ht);

  // Remplissage totale de ht et teste que les 3 derniers mots du N_gramme sont Test9998 Test9999 Test9999
  for (int i = 0; i < 10000; i++)
  {
    sprintf(wordi, "TEST%d", i);
    sequence_addWord(wordi, ht);
    sequence_progress();
    sequence_addWord(wordi, ht);
    sequence_progress();

    // Test que le retour de sequence_nextwor() est exactement le dernier mot qu'on a ajouté à la sequence dans la boucle
    dernierMot = sequence_nextWord();
    if (strcmp(dernierMot, wordi))
    {
      fprintf(stderr, "5 - Erreur dans le test intensifs (Fonctions concernée : sequence_nextWord() ) - Mot dans la table au lieu de %s: %s\n", wordi, dernierMot);
      strhash_free(ht);
      return 1;
    }
  }

  // Test que la sequence se compose des trois dernier mots ajouté en boocle
  if (!strcmp(sequence_printInTab(), "Test9998 Test9999 Test9999"))
  {
    fprintf(stderr, "6 - Erreur lors du remplissage intensif du ht et N_gramme.\nRésultat attendu : Test9998 Test9999 Test9999\nRésultat affiché : %s\n", sequence_printInTab());
    strhash_free(ht);
    return 1;
  }

  // Test si le nombre de mot ajoutés dans la table est exactement 10000 (le nombre de sequence_addWord dans la boucle)
  strhash_analyse(ht, &hstat);
  if (hstat.nbWord != 10000)
  {
    fprintf(stderr, "7 - Erreur dans sequence_addWord  - Mot dans la table au lieu de 10000 : %d\n", hstat.nbWord);
    strhash_free(ht);
    return 1;
  }

  // S'assurer que le dernier mot dans sequence_nextWord est exactement le dernier dans notre sequence
  dernierMot = sequence_nextWord();
  if (!strcmp(dernierMot, "Test9999"))
  {
    fprintf(stderr, "8 - Erreur dans le test intensifs - Mot dans la table au lieu de Test9999: %s\n", dernierMot);
    strhash_free(ht);
    return 1;
  }

  strhash_free(ht);

  return 0;
}

// Fonction de recherche
int rechercheInt_TabD(const void *element_recherche, const void *element_TabD)
{
  const int *val_recherche = (const int *)element_recherche;
  const int *val_tab = (const int *)element_TabD;

  return (*val_recherche == *val_tab) ? 1 : 0;
}

int rechercheChar_TabD(const void *element_recherche, const void *element_TabD)
{
  const char *val_recherche = (const char *)element_recherche;
  const char *val_tab = (const char *)element_TabD;

  return (strcmp(val_recherche, val_tab) == 0) ? 1 : 0;
}
void AfficherElementChar_TabD(const void *element)
{
  const char *val_char = (const char *)element;
  printf("%c\n", *val_char);
}

void AfficherElementInt_TabD(const void *element)
{
  const int *val_int = (const int *)element;
  printf("%d\n", *val_int);
}

// Test du module tableau dynamique
int test_TabD()
{

  // Création du tableau dynamique
  struct table_D *tableau1 = creer_TabD(20, 2); // Taille max initiale = 20, facteur de redimensionnement = 2.
  int i = 20, i1 = 2, i3 = 1;
  char c = 'A', c1 = '1', c2 = '2';
  int position;

  // Vérifie que le tableau a bien été créé
  if (tableau1 == NULL)
  {
    fprintf(stderr, "1 - Erreur lors du création du tableau\n");
    return 1;
  }

  // Vérification des caractéristiques initiales taille max, taille et facteur de redimension
  if (tableau1->taille_max != 20)
  {
    fprintf(stderr, "2 - La taille max du tableau est %d au lieu de 20\n", tableau1->taille_max);
    detruire_TabD(tableau1);
    return 1;
  }
  if (tableau1->taille != 0)
  {
    fprintf(stderr, "3 - La taille du tableau est %d au lieu de 0\n", tableau1->taille);
    detruire_TabD(tableau1);
    return 1;
  }
  if (tableau1->facteur != 2)
  {
    fprintf(stderr, "4 - Le facteur du tableau est %d au lieu de 2\n", tableau1->facteur);
    detruire_TabD(tableau1);
    return 1;
  }

  // Maj taille
  majtaille_TabD(tableau1, 56);
  if (tableau1->taille_max != 80)
  {
    fprintf(stderr, "5 - La taille max du tableau est %d au lieu de 80\n", tableau1->taille_max);
    detruire_TabD(tableau1);
    return 1;
  }
  if (tableau1->taille != 0)
  {
    fprintf(stderr, "6 - La taille du tableau est %d au lieu de 0\n", tableau1->taille);
    detruire_TabD(tableau1);
    return 1;
  }
  if (tableau1->facteur != 2)
  {
    fprintf(stderr, "7 - Le facteur du tableau est %d au lieu de 2\n", tableau1->facteur);
    detruire_TabD(tableau1);
    return 1;
  }

  // Pointeur générique pour manipuler les adresses des éléments.
  void *ptr = &i;

  // Ajout d'éléments de différents types (int et char) à différentes positions

  ajoutElement_TabD(tableau1, ptr, 0); // Ajout de l'entier 20 à la position 0

  if (tableau1->taille != 1)
  {
    fprintf(stderr, "8 - Probléme dans la fonction ajoutElement_TabD() -  La taille du tableau est %d au lieu de 1\n", tableau1->taille);
    detruire_TabD(tableau1);
    return 1;
  }

  if (ptr != lireElement_TabD(tableau1, 0))
  {
    fprintf(stderr, "9 - Probléme dans la fonction ajoutElement_TabD() -  Element pas trouvé en posistion 0 \n");
    detruire_TabD(tableau1);
    return 1;
  }

  if ((position = rechercheElement_TabD(&rechercheInt_TabD, ptr, tableau1)) != 0)
  {
    fprintf(stderr, "10 - Probléme dans la fonction rechercheElement_TabD() - Position de l'entier 0 est %d au lieu de 0  --- Taille : %d  --- Taille_max : %d\n", position, tableau1->taille, tableau1->taille_max);
    detruire_TabD(tableau1);
    return 1;
  }

  ptr = &c;
  // Ajout du caractère "A" à la position 1
  ajoutElement_TabD(tableau1, ptr, 1);

  if (tableau1->taille != 2)
  {
    fprintf(stderr, "11 - Probléme dans la fonction ajoutElement_TabD() - La taille du tableau est %d au lieu de 2\n", tableau1->taille);
    detruire_TabD(tableau1);
    return 1;
  }
  if ((position = rechercheElement_TabD(&rechercheChar_TabD, ptr, tableau1)) != 1)
  {
    fprintf(stderr, "12 - Probléme dans la fonction ajoutElement_TabD() - Position du caractere A est %d au lieu de 1\n", position);
    detruire_TabD(tableau1);
    return 1;
  }

  // Ajout de l'entier 2 à la position 3
  ptr = &i1;
  ajoutElement_TabD(tableau1, ptr, 3);
  if ((position = rechercheElement_TabD(&rechercheChar_TabD, ptr, tableau1)) != 3)
  {
    fprintf(stderr, "13 - Probléme dans la fonction ajoutElement_TabD() - Position de l'entier 2 est %d au lieu de 3\n", position);
    detruire_TabD(tableau1);
    return 1;
  }

  // Ajout du caractere 2 à la position 86
  ptr = &c2;
  ajoutElement_TabD(tableau1, ptr, 86);
  if (tableau1->taille_max != 160)
  {
    fprintf(stderr, "14 - Probléme dans la fonction ajoutElement_TabD() - La taille max du tableau est %d au lieu de 160\n", tableau1->taille);
    detruire_TabD(tableau1);
    return 1;
  }
  if (tableau1->taille != 87)
  {
    fprintf(stderr, "15 - Probléme dans la fonction ajoutElement_TabD() - La taille du tableau est %d au lieu de 87\n", tableau1->taille);
    detruire_TabD(tableau1);
    return 1;
  }
  if ((position = rechercheElement_TabD(&rechercheChar_TabD, ptr, tableau1)) != 86)
  {
    fprintf(stderr, "16 - Probléme dans la fonction ajoutElement_TabD() - Position du caractere 2 est %d au lieu de 86\n", position);
    detruire_TabD(tableau1);
    return 1;
  }

  // Ajout de l'entier 1 dans la fin du tableau
  ptr = &i3;
  ajoutenDernier_TabD(tableau1, ptr);

  if ((position = rechercheElement_TabD(&rechercheChar_TabD, ptr, tableau1)) != 87)
  {
    fprintf(stderr, "17 - Probléme dans la fonction ajoutElement_TabD() - Position du caractere 2 est %d au lieu de 87\n", position);
    detruire_TabD(tableau1);
    return 1;
  }

  // Ajout de l'entier 1 dans la fin du tableau
  int t = tableau1->taille, t_max = tableau1->taille_max;
  i1 = 10;
  ptr = &i1;
  ajoutenDernier_TabD(tableau1, ptr);

  if (tableau1->zone[2] != lireElement_TabD(tableau1, 2))
  {
    fprintf(stderr, "18 - Probléme dans la fonction ajoutenDernier_TabD() -  Element pas trouvé en position 2\n");
    detruire_TabD(tableau1);
    return 1;
  }

  if (tableau1->taille != t + 1)
  {
    fprintf(stderr, "19 - Probléme dans la fonction ajoutElement_TabD() - La taille du tableau est %d au lieu de %d\n", tableau1->taille, t);
    detruire_TabD(tableau1);
    return 1;
  }

  detruire_TabD(tableau1);
  return 0;
}

// Test du module tableau dynamique
int test_AP()
{

  // Création d'une table de hachage de taille définie par la constante ENTREE
  struct strhash_table *ht = strhash_create(ENTREE);

  // Initialisation de la séquence avec la table de hachage
  sequence_initialise(ht);
  sequence_itStart(); // Démarre l'itérateur de la séquence

  // Ajout de plusieurs mots à la séquence et progression à chaque étape
  sequence_addWord("Aymen", ht); // Ajoute "Aymen" à la séquence
  sequence_progress();           // Avance dans la séquence (pour indiquer que l'ajout est fait)

  sequence_addWord("Nachid", ht); // Ajoute "Nachid" à la séquence
  sequence_progress();            // Avance dans la séquence

  sequence_addWord("Casa", ht); // Ajoute "Casa" à la séquence
  sequence_progress();          // Avance dans la séquence

  // Création de l'arbre de prédiction (initialisation de la racine)
  struct Noeud *racine = creerRacine_AP();
  if (racine == NULL)
  {
    fprintf(stderr, "Erreur 1: La racine est NULL.\n"); // Vérification si la racine a bien été créée
    return 1;                                           // Retourne 1 si la racine est NULL
  }
  if (racine->fils->taille != 0)
  { // Vérification si le nombre de fils de la racine est initialement 0
    fprintf(stderr, "Erreur 2: racine->fils->taille est %d au lieu de 0.\n", racine->fils->taille);
    return 1; // Retourne 1 si l'erreur est détectée
  }

  // Ajout de mots au nœud racine (avec test de l'ajout)
  ajouterMot_AP(racine, "mon");
  ajouterMot_AP(racine, "test");
  ajouterMot_AP(racine, "perso");
  ajouterMot_AP(racine, "bon");

  if (racine->fils->taille != 4)
  { // Vérification que 4 mots ont bien été ajoutés sous la racine
    fprintf(stderr, "Erreur 3: racine->fils->taille est %d au lieu de 4.\n", racine->fils->taille);
    return 1; // Retourne 1 si l'erreur est détectée
  }

  // Recherche de la séquence qui n'existe pas dans l'arbre (Aymen Nachid Casa)
  struct Noeud *resultat_noeud = rechercher_completer_ngramme_AP(racine);
  if (resultat_noeud == NULL)
  {
    fprintf(stderr, "Erreur 4: Le resultat_noeud est NULL.\n"); // Vérifie si le résultat est NULL
  }
  if (resultat_noeud->mot == NULL)
  {
    fprintf(stderr, "Erreur 5: Le resultat_noeud->mot est NULL.\n"); // Vérifie si le mot du nœud trouvé est NULL
  }
  if (racine->fils->taille != 5)
  { // Vérification que racine a maintenant 5 fils au lieu de 4
    fprintf(stderr, "Erreur 6: racine->fils->taille est %d au lieu de 5.\n", racine->fils->taille);
    return 1; // Retourne 1 si l'erreur est détectée
  }
  // Nouveau noeud ajouté
  struct Noeud *nouveau_noeud = (struct Noeud *)lireElement_TabD(racine->fils, 4);

  if (strcmp(nouveau_noeud->mot, "Aymen"))
  {
    fprintf(stderr, "Erreur 7: dans rechercher_completer_ngramme_AP() - on a %s au lieu de Aymen\n", nouveau_noeud->mot);
    return 1; // Retourne 1 si l'erreur est détectée
  }
  struct Noeud *nouveau_noeud_niveau_2 = (struct Noeud *)lireElement_TabD(nouveau_noeud->fils, 0);
  if (strcmp(nouveau_noeud_niveau_2->mot, "Nachid"))
  {
    fprintf(stderr, "Erreur 8: dans rechercher_completer_ngramme_AP() - on a %s au lieu de Nachid\n", nouveau_noeud_niveau_2->mot);
    return 1; // Retourne 1 si l'erreur est détectée
  }
  struct Noeud *nouveau_noeud_niveau_3 = (struct Noeud *)lireElement_TabD(nouveau_noeud_niveau_2->fils, 0);
  if (strcmp(nouveau_noeud_niveau_3->mot, "Casa"))
  {
    fprintf(stderr, "Erreur 9: dans rechercher_completer_ngramme_AP() - on a %s au lieu de Casa\n", nouveau_noeud_niveau_3->mot);
    return 1; // Retourne 1 si l'erreur est détectée
  }
  if (nouveau_noeud_niveau_3 != resultat_noeud)
  {
    fprintf(stderr, "Erreur 10: dans rechercher_completer_ngramme_AP() - on a le mauvais noeud final.\n");
    return 1; // Retourne 1 si l'erreur est détectée
  }

  // Recherche 2 de la séquence existante dans l'arbre
  struct Noeud *noeud_existant = rechercher_completer_ngramme_AP(racine); // Nouvelle recherche du N-gramme
  if (noeud_existant == NULL)
  {
    fprintf(stderr, "Erreur 11: Le noeud_existant est NULL.\n"); // Vérifie si le deuxième résultat est NULL
  }
  if (noeud_existant->mot == NULL)
  {
    fprintf(stderr, "Erreur 12: Le noeud_existant->mot est NULL.\n"); // Vérifie si le mot du second nœud trouvé est NULL
  }
  if (strcmp(noeud_existant->mot, nouveau_noeud_niveau_3->mot))
  {
    fprintf(stderr, "Erreur 13: Le noeud_existant->mot est incorrect.\n"); // Vérifie si le mot du second nœud trouvé est NULL
  }
  // Vérification si le dernier nœud trouvé au second passage contient le pointeur du dernier noeud de la sequence
  if (noeud_existant != nouveau_noeud_niveau_3)
  {
    fprintf(stderr, "Erreur 14: Erreur fonction recherche.\n"); // Si les résultats sont différents, il y a une erreur
  }

  // Effectuer des recherches supplémentaires pour voir si le nombre d'occurence du mot "test" est ajouté puis incrementé
  rechercherMot_AP(noeud_existant, "test");
  rechercherMot_AP(noeud_existant, "test");
  rechercherMot_AP(noeud_existant, "test");
  rechercherMot_AP(noeud_existant, "test");

  // Recherche du mot "Cool", qui n'est pas dans l'arbre
  rechercherMot_AP(noeud_existant, "Cool");

  // Récupération et affichage du nœud ayant le plus grand nombre d'occurrences parmi les fils
  struct Noeud *resultat_noeud3 = (struct Noeud *)lireElement_TabD(noeud_existant->fils, 0);

  if (resultat_noeud3->occurrences != 4)
  { // Vérifie si le nombre d'occurrences du mot est correct
    fprintf(stderr, "Erreur 15: noeud_existant->occurrences, Nombre d'occurence est : %d au lieu de 4.\n", resultat_noeud3->occurrences);
  }
  resultat_noeud3 = (struct Noeud *)lireElement_TabD(noeud_existant->fils, 1); // Cool
  if (resultat_noeud3->occurrences != 1)
  { // Vérifie si le nombre d'occurrences du mot est correct
    fprintf(stderr, "Erreur 16: noeud_existant->occurrences, Nombre d'occurence est : %d au lieu de 1.\n", resultat_noeud3->occurrences);
  }

  // Libération de la mémoire allouée pour l'arbre de prédiction
  detruire_arbre(racine);
  strhash_free(ht); // Libère la mémoire de la table de hachage
  return 0;         // Retourne 0 pour indiquer que le test s'est terminé avec succès
}

// Test entree sortie
int test_E_S()
{

  return 0; // Retourne 0 pour indiquer que le test s'est terminé avec succès
}
