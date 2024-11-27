# Projet : Test de Modules de Gestion de Séquences et Tableaux Dynamiques

## Description

Ce projet teste deux modules principaux en C : **gestion des séquences** et **tableaux dynamiques**. Le programme inclut des tests automatisés pour vérifier le bon fonctionnement des fonctionnalités liées à l'ajout d'éléments, la gestion de la mémoire et la recherche d'éléments dans ces structures de données.

## Modules

1. **Module Séquence** : Ce module gère la séquence de mots et la table de hachage associée.
   - **Fonctionnalités** : 
     - Ajout de mots dans la séquence.
     - Initialisation de la séquence.
     - Gestion des N-grammes (série de mots consécutifs).
     - Fonction de récupération du dernier mot ajouté.
     - Tests de performance avec des ajouts massifs de mots.

2. **Module Tableau Dynamique** : Ce module implémente un tableau dynamique qui ajuste sa taille automatiquement lorsque la capacité est dépassée.
   - **Fonctionnalités** : 
     - Ajout d'éléments à différentes positions dans le tableau.
     - Recherche d'éléments dans le tableau.
     - Redimensionnement du tableau.
     - Ajout d'éléments à la fin du tableau.
     - Tests de performance avec ajout massif d'éléments.

## Fonctionnement

### Test du module Séquence

Le module Séquence utilise une table de hachage pour stocker les mots. Les tests suivants sont effectués :

1. **Initialisation de la Séquence** : Le test vérifie que la table de hachage a bien été créée et que la séquence commence correctement avec un mot vide.
2. **Ajout de mots** : Des mots sont ajoutés à la séquence et il est vérifié que les mots sont bien présents dans la séquence.
3. **Test intensif** : Un grand nombre de mots est ajouté à la séquence, et il est vérifié que la séquence fonctionne toujours correctement même avec des milliers de mots.
4. **Test de la récupération du dernier mot** : Le programme s'assure que le dernier mot ajouté est correctement récupéré.

### Test du module Tableau Dynamique

Le module Tableau Dynamique implémente une structure de données permettant de stocker des éléments dans un tableau qui redimensionne automatiquement sa capacité lorsque nécessaire.

Les tests suivants sont effectués :

1. **Création du tableau dynamique** : Le test vérifie que le tableau dynamique est créé avec les caractéristiques correctes (taille initiale, facteur de redimensionnement).
2. **Ajout d'éléments** : Des éléments de types différents (entiers, caractères) sont ajoutés à différentes positions dans le tableau. La taille du tableau est vérifiée après chaque ajout.
3. **Redimensionnement du tableau** : Le test s'assure que la taille maximale du tableau est mise à jour lors de l'ajout d'éléments au-delà de la capacité initiale.
4. **Recherche d'éléments** : Le programme vérifie que les éléments ajoutés peuvent être retrouvés à leur position correcte dans le tableau.
5. **Ajout à la fin du tableau** : Des éléments sont ajoutés à la fin du tableau pour tester le redimensionnement automatique et la mise à jour de la taille.

## Installation

### Prérequis

- Un compilateur C (par exemple, GCC)
- Une version de `make` pour la gestion de la compilation (si vous utilisez un Makefile).

### Compilation

1. **Cloner le projet** :
   ```bash
   git clone https://votre-repository.git
   cd nom_du_dossier
   ```

2. **Compiler le projet** :
   Si vous utilisez un Makefile :
   ```bash
   make
   ```
   Si vous compilez manuellement :
   ```bash
   gcc -o test test.c -lssl -lcrypto
   ```

3. **Exécuter les tests** :
   ```bash
   ./test
   ```

Les résultats des tests seront affichés dans la sortie standard. Si un test échoue, un message d'erreur sera affiché avec les détails.

## Structure du Projet

Le projet se compose des fichiers suivants :

- `test.c` : Contient les tests pour les modules de séquence et de tableau dynamique.
- `hash.h`, `hash.c` : Implémentation des fonctions de hachage et des tables de hachage.
- `list.h`, `list.c` : Définition des structures et fonctions liées aux listes (si utilisées).
- `sequence.h`, `sequence.c` : Définition et implémentation des fonctions pour la gestion des séquences.
- `tableau_dyn.h`, `tableau_dyn.c` : Définition et implémentation des fonctions pour les tableaux dynamiques.

## Fonctionnalités

- **Gestion des Séquences** : Le programme permet de gérer une séquence de mots stockée dans une table de hachage.
- **Tableau Dynamique** : Le tableau dynamique permet d'ajouter, de rechercher et de supprimer des éléments tout en redimensionnant automatiquement lorsque la capacité maximale est atteinte.

## Tests

Les tests effectués dans ce programme visent à vérifier la robustesse et l'efficacité des fonctions principales des modules de séquence et de tableau dynamique.

Les tests sont basés sur les éléments suivants :
- Création d'objets (tableau dynamique, table de hachage).
- Ajout, recherche et suppression d'éléments.
- Vérification du bon fonctionnement des redimensionnements de tableau.
- Tests de performance avec des ajouts massifs d'éléments.

## Contribution

Les contributions sont les bienvenues ! Si vous souhaitez améliorer ce projet ou signaler un bug, vous pouvez créer une nouvelle **issue** ou envoyer une **pull request**.


---

### Notes supplémentaires :
- Les tests sont automatisés et les erreurs sont affichées clairement pour aider à identifier les problèmes dans l'implémentation des modules.
- Vous pouvez ajouter d'autres tests pour valider davantage de cas limites ou des fonctionnalités spécifiques de ces modules.
#   T o u L a M e c h e 
 
 
