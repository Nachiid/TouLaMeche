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
#include "error.h"
#include "sequence.h"
#include "tableau_dyn.h"
#include "arbre_prediction.h"
#include "arbre_io.h"

// Test du module sequence

int test_sequence()
{
  // Création des structures nécessaires
  struct strhash_table *ht = strhash_create(ENTREE);
  struct strhash_stat hstat;
  const char *dernierMot = NULL;

  // Vérification de la création de la table
  if (ht == NULL)
  {
    error_print(ERR_HASH, "TEST", "Erreur lors de la création de la table de hachage.");
    return 1;
  }

  // Initialisation de la sequence (N-gramme de taille 3)
  Sequence *seq = sequence_creer(3, ht);

  // Test de nombre de mot après initialisation (doit contenir le mot vide "")
  strhash_analyse(ht, &hstat);
  if (hstat.nbWord != 1U)
  {
    fprintf(stderr, "2 - Erreur dans sequence_initialise - Mots dans la table : %d (attendu : 1)\n", hstat.nbWord);
    sequence_detruire(seq);
    strhash_free(ht);
    return 1;
  }

  // Test que le mot actuel est bien le mot vide initial
  dernierMot = sequence_nextWord(seq);
  if (strcmp(dernierMot, "") != 0)
  {
    fprintf(stderr, "3 - Erreur : le mot initial devrait être vide, reçu : [%s]\n", dernierMot);
    sequence_detruire(seq);
    strhash_free(ht);
    return 1;
  }

  // Test d'ajout de 4 mots (pour une séquence de taille 3)
  // On s'attend à ce que TEST0 disparaisse et qu'il reste TEST1 TEST2 TEST3
  char wordi[256];
  for (int i = 0; i < 4; i++)
  {
    sprintf(wordi, "TEST%d", i);
    sequence_pushWord(seq, wordi, ht); // Remplace addWord + progress
  }

  // Test du contenu de la séquence
  char *resultat = sequence_printInTab(seq);
  if (strcmp(resultat, "TEST1 TEST2 TEST3 ") != 0) // Note l'espace final de ta fonction printInTab
  {
    fprintf(stderr, "4 - Erreur contenu sequence.\nAttendu : TEST1 TEST2 TEST3 \nAffiché  : %s\n", resultat);
    sequence_detruire(seq);
    strhash_free(ht);
    return 1;
  }

  // Réinitialisation pour test intensif
  strhash_reset(ht);
  sequence_detruire(seq); // On libère l'ancienne
  seq = sequence_creer(3, ht);

  // Test intensif : 10 000 ajouts
  for (int i = 0; i < 10000; i++)
  {
    sprintf(wordi, "TEST%d", i);
    sequence_pushWord(seq, wordi, ht);

    // Vérification immédiate du dernier mot ajouté
    dernierMot = sequence_nextWord(seq);

    // Attention : après pushWord, sequence_nextWord pointe sur la case vide suivante (initialisée à "")
    // Pour avoir le mot qu'on vient de poser, il faudrait regarder l'index (pos - 1)
    // Mais testons si le mot est bien présent via l'itérateur à la fin du cycle
  }

  //  Test final après 10 000 ajouts
  // Les 3 derniers mots doivent être TEST9997 TEST9998 TEST9999
  resultat = sequence_printInTab(seq);
  if (strcmp(resultat, "TEST9997 TEST9998 TEST9999 ") != 0)
  {
    fprintf(stderr, "6 - Erreur test intensif.\nAttendu : TEST9997 TEST9998 TEST9999 \nAffiché  : %s\n", resultat);
    sequence_detruire(seq);
    strhash_free(ht);
    return 1;
  }

  //  Vérification du nombre de mots uniques dans la table de hachage
  strhash_analyse(ht, &hstat);
  if (hstat.nbWord != 10001)
  {
    fprintf(stderr, "7 - Erreur : nbWord dans ht = %d (attendu : 10001)\n", hstat.nbWord);
    sequence_detruire(seq);
    strhash_free(ht);
    return 1;
  }

  // Nettoyage final
  sequence_detruire(seq);
  strhash_free(ht);

  printf("Tests du module Sequence : REUSSIS\n");
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
  char c = 'A', c2 = '2';
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
  int t = tableau1->taille;
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

  printf("Tests du module tableau dynamique : REUSSIS\n");
  return 0;
}

// Test du module arbre de prediction
int test_AP()
{
  // 1. Création de la table de hachage
  struct strhash_table *ht = strhash_create(ENTREE);

  // 2. Initialisation de la séquence (Taille 3 pour "Aymen Nachid Casa")
  // On utilise le nouveau constructeur dynamique
  Sequence *seq = sequence_creer(3, ht);
  if (seq == NULL)
  {
    fprintf(stderr, "Erreur : Impossible de créer la séquence.\n");
    return 1;
  }

  // 3. Ajout des mots à la séquence
  // sequence_pushWord remplace avantageusement addWord + progress
  sequence_pushWord(seq, "Aymen", ht);
  sequence_pushWord(seq, "Nachid", ht);
  sequence_pushWord(seq, "Casa", ht);

  // 4. Création de l'arbre (Racine)
  struct Noeud *racine = creerRacine_AP(ht);
  if (racine == NULL)
  {
    fprintf(stderr, "Erreur 1: La racine est NULL.\n");
    sequence_detruire(seq); // Ne pas oublier de libérer ici aussi
    return 1;
  }

  // Vérification initiale des fils
  if (racine->fils->taille != 0)
  {
    fprintf(stderr, "Erreur 2: racine->fils->taille est %d au lieu de 0.\n", racine->fils->taille);
    return 1;
  }

  // Ajout de mots "manuels" au nœud racine
  ajouterMot_AP(racine, "mon");
  ajouterMot_AP(racine, "test");
  ajouterMot_AP(racine, "perso");
  ajouterMot_AP(racine, "bon");

  if (racine->fils->taille != 4)
  {
    fprintf(stderr, "Erreur 3: racine->fils->taille est %d au lieu de 4.\n", racine->fils->taille);
    return 1;
  }

  // 5. Recherche/Complétion du N-gramme
  // On passe maintenant 'seq' en deuxième argument
  struct Noeud *resultat_noeud = rechercher_completer_ngramme_AP(racine, seq);

  if (resultat_noeud == NULL)
  {
    fprintf(stderr, "Erreur 4: Le resultat_noeud est NULL.\n");
  }

  if (racine->fils->taille != 5)
  {
    fprintf(stderr, "Erreur 6: racine->fils->taille est %d au lieu de 5.\n", racine->fils->taille);
    return 1;
  }

  // Vérification de la structure créée dans l'arbre
  struct Noeud *nouveau_noeud = (struct Noeud *)lireElement_TabD(racine->fils, 4);
  if (strcmp(nouveau_noeud->mot, "Aymen") != 0)
  {
    fprintf(stderr, "Erreur 7: on a %s au lieu de Aymen\n", nouveau_noeud->mot);
    return 1;
  }

  struct Noeud *nouveau_noeud_niveau_2 = (struct Noeud *)lireElement_TabD(nouveau_noeud->fils, 0);
  if (strcmp(nouveau_noeud_niveau_2->mot, "Nachid") != 0)
  {
    fprintf(stderr, "Erreur 8: on a %s au lieu de Nachid\n", nouveau_noeud_niveau_2->mot);
    return 1;
  }

  struct Noeud *nouveau_noeud_niveau_3 = (struct Noeud *)lireElement_TabD(nouveau_noeud_niveau_2->fils, 0);
  if (strcmp(nouveau_noeud_niveau_3->mot, "Casa") != 0)
  {
    fprintf(stderr, "Erreur 9: on a %s au lieu de Casa\n", nouveau_noeud_niveau_3->mot);
    return 1;
  }

  // 6. Test de recherche d'une séquence déjà existante
  // L'itérateur de 'seq' sera réinitialisé au début de la fonction
  struct Noeud *noeud_existant = rechercher_completer_ngramme_AP(racine, seq);

  if (noeud_existant != nouveau_noeud_niveau_3)
  {
    fprintf(stderr, "Erreur 14: La recherche n'a pas retrouvé le nœud existant.\n");
    return 1;
  }

  // 7. Apprentissage de mots suivants
  rechercherMot_AP(noeud_existant, "test");
  rechercherMot_AP(noeud_existant, "test");
  rechercherMot_AP(noeud_existant, "test");
  rechercherMot_AP(noeud_existant, "test");
  rechercherMot_AP(noeud_existant, "Cool");

  // Vérification des occurrences
  struct Noeud *fils_test = (struct Noeud *)lireElement_TabD(noeud_existant->fils, 0);
  if (fils_test->occurrences != 4)
  {
    fprintf(stderr, "Erreur 15: occurrences de 'test' : %d au lieu de 4.\n", fils_test->occurrences);
  }

  // 8. Nettoyage final
  detruire_arbre(racine);
  sequence_detruire(seq); // Libération de la structure sequence
  strhash_free(ht);

  printf("Tests du module Arbre de Prédiction : REUSSIS\n");
  return 0;
}

/**
 * TEST_IO - Test d'intégration complet du module d'entrée/sortie
 * Retourne 0 si TOUT est parfait, 1 si une erreur est détectée.
 */
int test_IO()
{
  const char *nom_fichier = "system_test_arbre.bin";

  // ==========================================
  // 1. PRÉPARATION DES DONNÉES (L'ARBRE SOURCE)
  // ==========================================
  struct strhash_table *ht = strhash_create(5000);
  struct Noeud *racine_orig = creerRacine_AP(ht);

  if (!racine_orig)
  {
    fprintf(stderr, "[ERREUR 1] Impossible de créer la racine originale.\n");
    return 1;
  }

  // On crée une structure en "Y" pour tester les embranchements
  // Racine -> "le" -> "chat" -> "mange"
  //                -> "chien" -> "dort"
  const char *p_le = strhash_wordAdd(ht, "le");
  const char *p_chat = strhash_wordAdd(ht, "chat");
  const char *p_chien = strhash_wordAdd(ht, "chien");
  const char *p_mange = strhash_wordAdd(ht, "mange");
  const char *p_dort = strhash_wordAdd(ht, "dort");

  // Montage de l'arbre
  // 1. Ajout de "le" à la racine
  ajouterMot_AP(racine_orig, p_le);
  // On récupère le nœud "le" qui est maintenant le premier fils de la racine
  struct Noeud *n_le = (struct Noeud *)lireElement_TabD(racine_orig->fils, 0);

  // 2. Ajout de "chat" et "chien" comme fils de "le"
  ajouterMot_AP(n_le, p_chat);
  ajouterMot_AP(n_le, p_chien);

  // On récupère les pointeurs pour descendre plus bas
  // "chat" est à l'index 0 de n_le, "chien" à l'index 1
  struct Noeud *n_chat = (struct Noeud *)lireElement_TabD(n_le->fils, 0);
  struct Noeud *n_chien = (struct Noeud *)lireElement_TabD(n_le->fils, 1);

  // 3. Ajout des terminaisons
  ajouterMot_AP(n_chat, p_mange); // "mange" sous "chat"
  ajouterMot_AP(n_chien, p_dort); // "dort" sous "chien"

  // Mise à jour manuelle des occurrences pour ton test
  n_le->occurrences = 100;
  n_chat->occurrences = 50;
  n_chien->occurrences = 30;
  // Note : Les fils de n_chat et n_chien auront 1 par défaut via creerNoeud_AP

  // ==========================================
  // 2. SAUVEGARDE
  // ==========================================
  sauvegarder_AP(racine_orig, nom_fichier);

  FILE *f_check = fopen(nom_fichier, "rb");
  if (!f_check)
  {
    fprintf(stderr, "[ERREUR 2] Le fichier binaire n'a pas été généré.\n");
    return 1;
  }
  fclose(f_check);
  // ==========================================
  // 3. CHARGEMENT
  // ==========================================
  struct Noeud *racine_chargee = charger_AP(nom_fichier, ht);
  if (!racine_chargee)
  {
    fprintf(stderr, "[ERREUR 3] Échec du chargement de l'arbre.\n");
    return 1;
  }
  // ==========================================
  // 4. VÉRIFICATION CROISÉE (HARD TEST)
  // ==========================================

  // A. Test de la Racine
  if (racine_chargee->mot == NULL || strcmp(racine_chargee->mot, "") != 0)
  {
    fprintf(stderr, "[ERREUR 4] La racine ne contient pas le mot vide.\n");
    return 1;
  }
  const char *mot_vide_hache = strhash_wordAdd(ht, "");

  if (racine_chargee->mot != mot_vide_hache)
  {

    fprintf(stderr, "[ERREUR 4] La racine n'utilise pas le pointeur unique du mot vide.\n");
    return 1;
  }

  // B. Test du premier niveau ("le")
  struct Noeud *c_le = (struct Noeud *)lireElement_TabD(racine_chargee->fils, 0);
  if (c_le->mot != p_le || c_le->occurrences != 100)
  {
    fprintf(stderr, "[ERREUR 5] Données corrompues pour le mot 'le'.\n");
    return 1;
  }
  // Verif adresse mémoire (doit être identique car même table de hachage)
  if (c_le->mot != n_le->mot)
  {
    fprintf(stderr, "[ERREUR 6] Problème d'unicité des pointeurs (Hash Table non respectée).\n");
    return 1;
  }

  // C. Test des embranchements ("chat" et "chien")
  if (c_le->fils->taille != 2)
  {
    fprintf(stderr, "[ERREUR 7] Nombre de fils incorrect pour 'le' (%d au lieu de 2).\n", c_le->fils->taille);
    return 1;
  }

  struct Noeud *c_chat = (struct Noeud *)lireElement_TabD(c_le->fils, 0);
  struct Noeud *c_chien = (struct Noeud *)lireElement_TabD(c_le->fils, 1);

  if (strcmp(c_chat->mot, "chat") != 0 || strcmp(c_chien->mot, "chien") != 0)
  {
    fprintf(stderr, "[ERREUR 8] Inversion ou corruption des fils de 'le'.\n");
    return 1;
  }

  // D. Test de la profondeur maximale ("mange" et "dort")
  struct Noeud *c_mange = (struct Noeud *)lireElement_TabD(c_chat->fils, 0);
  struct Noeud *c_dort = (struct Noeud *)lireElement_TabD(c_chien->fils, 0);

  if (c_mange->occurrences != 1 || c_dort->occurrences != 1)
  {
    fprintf(stderr, "[ERREUR 9] Erreur sur les feuilles de l'arbre.\n");
    return 1;
  }

  // ==========================================
  // 5. NETTOYAGE
  // ==========================================
  detruire_arbre(racine_orig);
  detruire_arbre(racine_chargee);
  strhash_free(ht);
  remove(nom_fichier);

  printf("Tests du module arbre io : REUSSIS\n");
  return 0;
}