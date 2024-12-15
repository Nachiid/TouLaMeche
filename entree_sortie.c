#include <stdio.h>
#include "entree_sortie.h"
#include "sequence.h" // Inclusion du fichier de la séquence de mots

int saisieTexte(const char *fichier_texte)
{

    FILE *mon_fichier = fopen(fichier_texte, "a");
    if (mon_fichier == -1)
    {
        perror("mon fichier n'as pas été correctement ouvert");
        return -1;
    }

    sequence_initialise(DICTIONNAIRE);
    int mot, ecris_lus, taille_ecrite, taille = 0;
    char chaine[256];
    struct Noeud *racine_actuelle = ARBRE;
    struct Noeud *Noeuds_retourne = NULL;

    mot = getchar();
    while (mot != EOF)
    {

        switch (mot)
        {
        case ' ':
            // Ajout du mot dans la sequence
            chaine[taille] = '\0'; // Definir la fin de la chaine
            sequence_addWord(chaine, DICTIONNAIRE);
            sequence_progress();
            // Ajout du mot dans le fichier de sortie
            ecris_lus = fwrite(chaine, taille, taille_ecrite, mon_fichier);
            if (taille_ecrite != ecris_lus)
            {
                // perror("Nombre de caractere ecris au lieu de %d est %d\n",taille_ecrite, ecris_lus);
                printf("Nombre de caractere ecris au lieu de %d est %d\n", taille_ecrite, ecris_lus);
                fclose(mon_fichier);
            }
            ecris_lus = fwrite(" ", 1, taille_ecrite, mon_fichier);
            if (taille_ecrite != ecris_lus)
            {
                perror("mon fichier n'as pas été correctement ouvert");
                fclose(mon_fichier);
            }
            mot = getchar();
            break;

        case '\t':
            // Ajout du mot dans la sequence
            chaine[taille] = '\0'; // Definir la fin de la chaine
            sequence_addWord(chaine, DICTIONNAIRE);
            sequence_progress();
            // Recherche du mot dans l'arbre
            Noeuds_retourne = MotMaxOccurrences_AP(rechercher_completer_ngramme_AP(racine_actuelle));
            // Affichage du mot avec le + d'occurence
            afficherMot_AP((void *)Noeuds_retourne->fils);
            Noeuds_retourne = (struct Noeud *)Noeuds_retourne;
            if ((mot = getchar()) == '\n')
            {
                return 0;
            }
            racine_actuelle = Noeuds_retourne;
            break;

        default:
            putchar(mot);
            chaine[taille] = mot;
            taille++;
            mot = getchar();
            break;
        }
    }
    return 0;
}

int Ajout_texte(const char *fichier_texte)
{

    FILE *mon_fichier = fopen(fichier_texte, "a");
    if (mon_fichier == -1)
    {
        perror("mon fichier n'as pas été correctement ouvert");
        return -1;
    }

    //charger la base de donnée dans un text 
    //le texte doit etre convertie en arbre de base de donnée

    return 0;
}