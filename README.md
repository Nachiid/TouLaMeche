# **Projet : Test de Modules de Gestion de Séquences et Tableaux Dynamiques**

## **Description**

Ce projet a pour objectif de tester deux modules principaux en langage C : 
1. **Gestion des séquences** : Permet la manipulation de suites de mots en utilisant une table de hachage.
2. **Tableaux dynamiques** : Implémente une structure de tableau à capacité ajustable, adaptée aux besoins en mémoire.

Les fonctionnalités principales incluent des tests automatisés pour valider l'ajout d'éléments, la gestion de la mémoire, ainsi que la recherche d'éléments dans ces structures de données.

---

## **Modules**

### **1. Module Séquence**
Gère des séquences de mots en utilisant une table de hachage pour le stockage.

#### **Fonctionnalités :**
- Initialisation de la séquence et de la table de hachage associée.
- Ajout de mots dans la séquence.
- Gestion des N-grammes (groupes de mots consécutifs).
- Récupération du dernier mot ajouté.
- Tests de performance pour des ajouts massifs de mots.

---

### **2. Module Tableau Dynamique**
Implémente un tableau dynamique redimensionnable pour stocker des éléments divers.

#### **Fonctionnalités :**
- Ajout d'éléments à différentes positions du tableau.
- Redimensionnement automatique lorsque la capacité maximale est atteinte.
- Recherche et récupération d'éléments par index.
- Ajout d'éléments à la fin du tableau.
- Tests de robustesse avec des ajouts massifs d'éléments.

---

## **Fonctionnement**

### **Tests pour le module Séquence**
1. **Initialisation** : Vérification de la création correcte de la séquence et de la table de hachage.
2. **Ajout de mots** : Validation que chaque mot ajouté est correctement enregistré.
3. **Test intensif** : Ajout de milliers de mots pour s'assurer de la robustesse.
4. **Récupération du dernier mot** : Vérification que le dernier mot ajouté peut être récupéré avec précision.

---

### **Tests pour le module Tableau Dynamique**
1. **Création** : Validation des paramètres initiaux (taille, capacité, etc.).
2. **Ajout d'éléments** : Ajout d'éléments divers et vérification de leur insertion correcte.
3. **Redimensionnement** : Test du fonctionnement lors du dépassement de la capacité initiale.
4. **Recherche d'éléments** : Vérification de l'accès aux éléments selon leur position.
5. **Ajout en fin de tableau** : Validation du redimensionnement lors d'ajouts successifs.

---

## **Installation**

### **Prérequis**
- Un compilateur C (comme GCC).
- `make` pour la gestion des builds (si un Makefile est utilisé).

### **Étapes**
1. **Cloner le projet** :
   ```bash
   git clone https://github.com/votre-repository.git
   cd votre-dossier
   ```

2. **Compiler le projet** :
   - Avec Makefile :
     ```bash
     make
     ```
   - Sans Makefile :
     ```bash
     gcc -o test test.c -lssl -lcrypto
     ```

3. **Exécuter les tests** :
   ```bash
   ./test
   ```

---

## **Structure du Projet**

- **`test.c`** : Contient les tests unitaires pour les modules.
- **`hash.h`**, **`hash.c`** : Implémentation de la table de hachage.
- **`list.h`**, **`list.c`** : Fonctions liées aux listes (si utilisées).
- **`sequence.h`**, **`sequence.c`** : Gestion des séquences.
- **`tableau_dyn.h`**, **`tableau_dyn.c`** : Gestion des tableaux dynamiques.

---

## **Tests**

Les tests automatisés valident :
- La création des structures (tableaux dynamiques, séquences, etc.).
- L'ajout, la recherche et la suppression d'éléments.
- Le comportement des structures sous des charges importantes.

---

## **Contribution**

Les contributions sont encouragées ! Vous pouvez :
- Signaler un problème via une **issue**.
- Proposer des améliorations via une **pull request**.

---

## **Licence**

```text
Copyright © 2024 NACHID. All rights reserved.

Ce projet est mis à disposition pour consultation et apprentissage. Toute modification, redistribution ou utilisation commerciale du code sans autorisation expresse de l'auteur est strictement interdite.
```
