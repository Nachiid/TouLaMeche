# **Projet : Test de Modules de Gestion de Séquences, Tableaux Dynamiques et Arbre de Prédiction**

## **Etat : En cours de développement**

Ce projet a pour objectif de tester et développer plusieurs modules en C pour gérer des structures de données complexes. Ces modules incluent la **gestion des séquences**, les **tableaux dynamiques**, et un **arbre de prédiction**. Le programme inclut des tests pour valider les fonctionnalités implémentées. 

Actuellement, le projet est encore en construction, avec deux fonctionnalités principales restant à intégrer :
- **Module Entrées/Sorties** pour l'exécution du programme en mode terminal (saisie et sauvegarde des données).
- **Module d'Interface Graphique (IHM)** pour un affichage interactif utilisant GTK+.

---

## **Modules**

### 1. **Module Séquence**
**Raison de sa création :**
Le module Séquence a été conçu pour gérer des séries de mots (séquences) tout en permettant une récupération rapide des mots précédents. Cette structure est essentielle pour des systèmes de prédiction basés sur des N-grammes.

**Fonctionnalités :**
- **Gestion des séquences de mots** : Stockage et manipulation de séquences N-grammes.
- **Initialisation** : Création d'une séquence vide pour commencer.
- **Ajout de mots** : Ajoute des mots à une séquence et les gère efficacement.
- **Récupération du dernier mot** : Permet d'accéder au dernier mot ajouté dans la séquence.
- **Tests de performance** : Simulation d'ajouts massifs pour évaluer la robustesse du module.

---

### 2. **Module Tableau Dynamique**
**Raison de sa création :**
Ce module a été développé pour offrir une structure de données flexible permettant d'ajuster automatiquement sa taille en fonction du nombre d'éléments ajoutés. Il est utilisé pour des besoins généraux de stockage dynamique.

**Fonctionnalités :**
- **Redimensionnement automatique** : Le tableau s'adapte lorsque sa capacité maximale est atteinte.
- **Ajout d'éléments** : Permet d'ajouter des éléments à différentes positions dans le tableau.
- **Recherche** : Recherche rapide des éléments par leur index.
- **Gestion des types divers** : Supporte l'ajout d'éléments de différents types (par exemple : entiers, caractères, structures).

---

### 3. **Module Arbre de Prédiction**
**Raison de sa création :**
L'arbre de prédiction permet de gérer efficacement une base de données de mots et d'offrir des suggestions basées sur des séquences de mots (N-grammes). Ce module est au cœur du système de prédiction du programme.

**Fonctionnalités :**
- **Initialisation de l'arbre** : Création d'un arbre de prédiction avec une racine contenant un mot vide.
- **Ajout des mots** : Construction de l'arbre à partir d'un dictionnaire de mots ou d'une séquence d'entrées.
- **Prédiction de mots** : Propose le mot suivant le plus probable en fonction des séquences enregistrées.
- **Tests de performance** : Évaluation de la capacité de l'arbre à gérer de grandes bases de données.

---

## **Modules en cours de développement**

1. **Module Entrées/Sorties (Terminal)**  
   **Fonctionnalité prévue** :  
   - Gestion de la saisie et de la sauvegarde des textes en mode terminal.  
   - Permettre à l'utilisateur de saisir un texte mot par mot avec prédiction en temps réel via l'arbre de prédiction.
   - Enregistrer les mots dans un fichier si la prédiction ne correspond pas.  
   **Statut :** En cours de développement.  

2. **Module Interface Graphique (IHM)**  
   **Fonctionnalité prévue** :  
   - Création d'une interface graphique interactive en utilisant GTK+ et Glade.  
   - Permettre à l'utilisateur de visualiser et manipuler les structures de données (séquences, arbres, tableaux dynamiques) avec une interface conviviale.  
   **Statut :** En cours de conception.

---

## **Installation**

### Prérequis
- Un compilateur C (par exemple, GCC).
- `make` pour la gestion de la compilation (si vous utilisez un Makefile).
- GTK+ installé (si vous voulez tester l'IHM lorsqu'elle sera implémentée).

### Compilation
1. **Cloner le projet** :
   ```bash
   git clone https://github.com/Nachiid/TouLaMeche
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

---

## **Structure du Projet**

Le projet est organisé comme suit :

- **test.c** : Contient les tests pour valider les fonctionnalités des modules.
- **sequence.h**, **sequence.c** : Gestion des séquences de mots (N-grammes).
- **tableau_dyn.h**, **tableau_dyn.c** : Gestion des tableaux dynamiques.
- **arbre_prediction.h**, **arbre_prediction.c** : Implémentation de l'arbre de prédiction.
- **entrees_sorties.h**, **entrees_sorties.c** (en cours) : Gestion des saisies utilisateur et des fichiers.
- **interface_gtk.h**, **interface_gtk.c** (en cours) : Création de l'IHM avec GTK+.

---

## **Tests**

Les tests incluent :
1. **Module Séquence** : Validation des fonctionnalités de stockage et récupération des séquences.
2. **Module Tableau Dynamique** : Tests de redimensionnement et de recherche d'éléments.
3. **Module Arbre de Prédiction** : Tests de précision et de performance pour la prédiction des mots.
4. **Tests intensifs** : Vérification de la robustesse avec de grandes quantités de données.

## **Licence**

```text
Copyright © 2024 NACHID. All rights reserved.

Ce projet est mis à disposition pour consultation et apprentissage. Toute modification, redistribution ou utilisation commerciale du code sans autorisation expresse de l'auteur est strictement interdite.
```