# TouLaMeche — N-gram Word Prediction Engine

> A lightweight, dependency-free word prediction engine written in C,  
> built around an N-gram trie, a string-interning hash table, and a generic dynamic array.

![Status](https://img.shields.io/badge/status-under%20development-orange)
![Language](https://img.shields.io/badge/language-C-blue)
![License](https://img.shields.io/badge/license-MIT-green)

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Module Reference](#module-reference)
  - [Hash Table — `hash.h`](#1-hash-table--hashh)
  - [Linked List — `list.h`](#2-linked-list--listh)
  - [Sequence — `sequence.h`](#3-sequence--sequenceh)
  - [Dynamic Array — `tableau_dyn.h`](#4-dynamic-array--tableau_dynh)
  - [Prediction Tree — `arbre_prediction.h`](#5-prediction-tree--arbre_predictionh)
  - [Serialization — `arbre_io.h`](#6-serialization--arbre_ioh)
- [Modules Under Development](#modules-under-development)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Running the Tests](#running-the-tests)
- [License](#license)

---

## Overview

**TouLaMeche** is a word prediction system that learns from text and suggests the most probable next word given a context window of N previous words (an N-gram).

The core idea is simple:

1. **Learn** — feed the engine a stream of words. It builds an N-ary trie where each root-to-node path encodes an N-gram, and tracks how often each word follows a given context.
2. **Predict** — give it a context (the last N−1 words); it returns the word with the highest observed frequency in that context, in **O(1)** time.

Every word is stored exactly once in a hash table (string interning). All other data structures — the trie nodes, the sequence buffer, the dynamic arrays — hold pointers into that table, keeping memory usage minimal and word comparison as fast as a pointer equality check.

---

## Architecture

```
Text stream
     │
     ▼
┌─────────────┐     intern      ┌──────────────────┐
│  Sequence   │ ──────────────► │  Hash Table (ht)  │
│ (N-gram buf)│                 │  unique char*     │
└──────┬──────┘                 └──────────────────┘
       │  context window                 ▲
       ▼                                 │ pointer
┌──────────────────────────────┐         │
│     Prediction Tree (trie)   │─────────┘
│                              │
│  root → w1 → w2 → w3        │
│              ↳ w2'→ w3'     │  children sorted by
│                              │  occurrence count ↓
│  best prediction = index [0] │  → O(1) lookup
└──────────────────────────────┘
       │
       ▼
  Serialization (binary file)
  save / load full trie
```

The child arrays inside the trie are maintained in **descending occurrence order** by `remonterNoeud_AP()` after every learning step, so the top prediction is always at index 0 without any sorting pass at query time.

---

## Module Reference

### 1. Hash Table — `hash.h`

**Files:** `hash.h`, `hash.c`

Implements a **string-interning open-bucket hash table**. Each unique word is stored exactly once; `strhash_wordAdd()` always returns the same pointer for the same string. This is the foundation that makes pointer-equality word comparison valid everywhere else in the project.

| Function | Description |
|---|---|
| `strhash_create(size)` | Allocate a new table with `size` buckets |
| `strhash_wordAdd(ht, word)` | Intern `word` and return its unique pointer |
| `strhash_wordDel(ht, word)` | Remove a word (invalidates all held pointers to it) |
| `strhash_reset(ht)` | Clear all entries; table remains usable |
| `strhash_free(ht)` | Reset and destroy the table |
| `strhash_analyse(ht, stat)` | Compute occupancy statistics (mean, stddev, min/max) |
| `strhash_display(ht)` | Print all interned words (debug) |

> **Key invariant:** strings are owned by the hash table. No other module ever calls `free()` on a word pointer.

---

### 2. Linked List — `list.h`

**Files:** `list.h`, `list.c`

A generic **singly-linked list** storing `void*` pointers. Used internally by the hash table as the collision-chain structure for each bucket.

| Function | Description |
|---|---|
| `list_create()` | Create an empty list |
| `list_insert(head, data)` | Prepend a node — O(1) |
| `list_append(head, data)` | Append a node — O(n) |
| `list_append2(&head, tail, data)` | Append with cached tail — O(1) |
| `list_orderedAppend(&head, cmp, data)` | Insert in sorted order, no duplicates |
| `list_remove(head, data)` | Remove first matching node |
| `list_headRemove(head)` | Remove head node — O(1) |
| `list_destroy(head)` | Free all nodes (data NOT freed) |
| `list_process(head, fct, param, &last)` | Apply a callback; early-stop supported |

---

### 3. Sequence — `sequence.h`

**Files:** `sequence.h`, `sequence.c`

A **fixed-size circular buffer** of interned string pointers, representing a sliding N-gram window over a text stream. Pushing a new word automatically evicts the oldest one.

```
 taille_max = 4  (N = 3)

 ┌────────┬────────┬────────┬────────┐
 │ TEST1  │ TEST2  │ TEST3  │  ←pos  │
 └────────┴────────┴────────┴────────┘
   oldest                    write head
```

| Function | Description |
|---|---|
| `sequence_creer(n, ht)` | Allocate a size-(n+1) circular buffer |
| `sequence_pushWord(seq, word, ht)` | Intern and push `word`, advance write head |
| `sequence_nextWord(seq)` | Peek at the word at the current write head |
| `sequence_itStart(seq)` | Reset iterator to the oldest entry |
| `sequence_itHasNext(seq)` | Check if more words remain to iterate |
| `sequence_itNext(seq)` | Return current word and advance iterator |
| `sequence_print(seq)` | Print non-empty words separated by ` / ` |
| `sequence_printInTab(seq)` | Serialize words into a static 256-byte buffer |
| `sequence_detruire(seq)` | Free the buffer (strings stay in hash table) |

---

### 4. Dynamic Array — `tableau_dyn.h`

**Files:** `tableau_dyn.h`, `tableau_dyn.c`

A **generic growable array** of `void*` pointers with a configurable multiplicative growth factor. Used as the child storage structure for every node in the prediction tree.

```
 taille = 3     taille_max = 8     facteur = 2

 ┌──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┐
 │  *0  │  *1  │  *2  │ NULL │ NULL │ NULL │ NULL │ NULL │
 └──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┘
```

| Function | Description |
|---|---|
| `creer_TabD(taille_max, facteur)` | Allocate a new dynamic array |
| `ajoutElement_TabD(t, elem, pos)` | Insert at `pos`, shifting right if occupied |
| `ajoutenDernier_TabD(t, elem)` | Append at logical end |
| `ecrireElement_TabD(t, elem, pos)` | Overwrite in-place or insert if out of bounds |
| `lireElement_TabD(t, pos)` | Read element at `pos` |
| `rechercheElement_TabD(cmp, elem, t)` | Linear search using a predicate callback |
| `majtaille_TabD(t, min)` | Grow buffer to at least `min` capacity |
| `AfficherElement_Tabd(pf, t, pos)` | Display element via callback |
| `detruire_TabD(t)` | Free the array (elements NOT freed) |

---

### 5. Prediction Tree — `arbre_prediction.h`

**Files:** `arbre_prediction.h`, `arbre_prediction.c`

The core of the engine. An **N-ary trie** where each root-to-leaf path encodes an N-gram. Each node stores an interned word pointer and an occurrence count. Child arrays are kept **sorted in descending order** of occurrence so the best prediction is always at index 0.

```
root ("")
 ├── "le"  (100)
 │    ├── "chat"  (50)
 │    │    └── "mange" (1)
 │    └── "chien" (30)
 │         └── "dort"  (1)
 └── "un"  (42)
      └── ...
```

| Function | Description |
|---|---|
| `creerRacine_AP(ht)` | Create the root node (empty-string word, count = 0) |
| `creerNoeud_AP(mot)` | Allocate a new node for `mot` (count starts at 1) |
| `rechercher_completer_ngramme_AP(root, seq)` | Walk/extend trie along N-gram path |
| `rechercherMot_AP(node, mot)` | Record an observed next word (increment or create) |
| `MotMaxOccurrences_AP(node)` | Return best prediction child — O(1) |
| `ajouterMot_AP(node, mot)` | Add a new child node |
| `remonterNoeud_AP(fils, index)` | Bubble-up to restore descending order |
| `comparerMot_AP(node_v, mot_v)` | Pointer-equality predicate for search |
| `detruire_arbre(node)` | Recursive post-order free (strings NOT freed) |
| `afficherArbre(root, level)` | Pretty-print the trie with indentation |

---

### 6. Serialization — `arbre_io.h`

**Files:** `arbre_io.h`, `sauvegarde_AP.c`

Binary **save and load** for the full prediction trie. The tree is serialized in pre-order (node before children). The root's empty-string sentinel is replaced by a zero-length marker so it can always be reconstructed via `creerRacine_AP()` on load.

**Binary record layout (per node):**
```
[ int:  word_length  ]   length in bytes including '\0', or 0 for NULL/empty
[ char: word[len]    ]   word bytes (only present when word_length > 0)
[ int:  occurrences  ]   observation count
[ int:  nb_children  ]   number of children that follow
... children records (recursive, same layout) ...
```

| Function | Description |
|---|---|
| `sauvegarder_AP(root, filename)` | Serialize the full trie to a binary file |
| `charger_AP(filename, ht)` | Deserialize and rebuild the trie from file |
| `normaliser_mot(str)` | Lowercase a word in-place (ASCII-safe) |

---

## Modules Under Development

### Terminal I/O — `entrees_sorties.h` *(in progress)*

Planned features:
- Word-by-word text input in terminal mode with real-time prediction display
- Automatic saving of unrecognized words to a training file
- Full round-trip: learn from a corpus file → predict → update model → save

### Graphical Interface — `interface_gtk.h` *(design phase)*

Planned features:
- Interactive GTK+ / Glade interface
- Live visualization of the sequence buffer and trie structure
- Real-time prediction display as the user types

---

## Project Structure

```
TouLaMeche/
│
├── hash.h / hash.c               # String-interning hash table
├── list.h / list.c               # Generic singly-linked list
├── sequence.h / sequence.c       # N-gram circular buffer
├── tableau_dyn.h / tableau_dyn.c # Generic growable array
├── arbre_prediction.h / .c       # N-ary prediction trie
├── arbre_io.h / sauvegarde_AP.c  # Binary serialization
├── error.h / error.c             # Centralized error handling
│
├── test.h / test.c               # Unit & integration tests
│
├── entrees_sorties.h / .c        # [IN PROGRESS] Terminal I/O
├── interface_gtk.h / .c          # [IN PROGRESS] GTK+ GUI
│
└── Makefile
```

---

## Getting Started

### Prerequisites

- GCC or any C99-compatible compiler
- `make`
- GTK+ 3 *(only required once the GUI module is integrated)*

### Build

```bash
# Clone the repository
git clone https://github.com/Nachiid/TouLaMeche
cd TouLaMeche

# Build with Makefile
make

# Or compile manually
gcc -o test test.c sequence.c tableau_dyn.c arbre_prediction.c \
    sauvegarde_AP.c hash.c list.c error.c -Wall -Wextra
```

---

## Running the Tests

The test suite covers all currently implemented modules:

```bash
./main
```

Expected output:

```
Tests du module Sequence : REUSSIS
Tests du module tableau dynamique : REUSSIS
Tests du module Arbre de Prédiction : REUSSIS
Tests du module arbre io : REUSSIS
fin du test
```

### What is tested

| Test | Module | What it covers |
|---|---|---|
| `test_sequence()` | Sequence | Creation, circular overwrite, 10 000-word stress test |
| `test_TabD()` | Dynamic Array | Creation, resizing, insertion, search, append |
| `test_AP()` | Prediction Tree | Node creation, trie extension, learning, sort invariant |
| `test_IO()` | Serialization | Full save/load round-trip, pointer interning after reload |
| `test_hash` *(standalone)* | Hash Table | Insert, dedup, delete, reset, 1 000 000-word stress test |

---

## License

```
MIT License

Copyright (c) 2024 NACHID

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```