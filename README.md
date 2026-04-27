# **Project: Testing Modules for Sequence Management, Dynamic Arrays, and Prediction Tree**

## **Status: Under Development**

This project aims to test and develop several C modules for managing complex data structures. These modules include **sequence management**, **dynamic arrays**, and a **prediction tree**. The program includes tests to validate the implemented functionalities.

Currently, the project is still under construction, with two main features remaining to be integrated:

* **Input/Output Module** for running the program in terminal mode (data input and saving).
* **Graphical User Interface (GUI) Module** for interactive display using GTK+.

---

## **Modules**

### 1. **Sequence Module**

**Purpose:**
The Sequence module was designed to manage sequences of words while allowing fast retrieval of previous words. This structure is essential for prediction systems based on N-grams.

**Features:**

* **Word sequence management**: Storage and manipulation of N-gram sequences.
* **Initialization**: Creation of an empty sequence to start with.
* **Adding words**: Efficiently adds words to a sequence.
* **Retrieving the last word**: Access to the last word added to the sequence.
* **Performance testing**: Simulation of massive insertions to evaluate module robustness.

---

### 2. **Dynamic Array Module**

**Purpose:**
This module was developed to provide a flexible data structure that automatically adjusts its size based on the number of elements added. It is used for general-purpose dynamic storage.

**Features:**

* **Automatic resizing**: The array adapts when its maximum capacity is reached.
* **Adding elements**: Allows insertion of elements at different positions in the array.
* **Search**: Fast lookup of elements by index.
* **Handling multiple data types**: Supports adding elements of different types (e.g., integers, characters, structures).

---

### 3. **Prediction Tree Module**

**Purpose:**
The prediction tree efficiently manages a database of words and provides suggestions based on word sequences (N-grams). This module is the core of the program’s prediction system.

**Features:**

* **Tree initialization**: Creation of a prediction tree with a root containing an empty word.
* **Adding words**: Building the tree from a dictionary or input sequences.
* **Word prediction**: Suggests the most probable next word based on stored sequences.
* **Performance testing**: Evaluation of the tree’s ability to handle large datasets.

---

## **Modules Under Development**

1. **Input/Output Module (Terminal)**
   **Planned Features:**

   * Handling user input and text saving in terminal mode.
   * Allowing the user to input text word by word with real-time prediction using the prediction tree.
   * Saving words to a file if the prediction does not match.
     **Status:** Under development.

2. **Graphical User Interface (GUI) Module**
   **Planned Features:**

   * Creation of an interactive graphical interface using GTK+ and Glade.
   * Allowing users to visualize and manipulate data structures (sequences, trees, dynamic arrays) through a user-friendly interface.
     **Status:** In design phase.

---

## **Installation**

### Prerequisites

* A C compiler (e.g., GCC).
* `make` for build management (if using a Makefile).
* GTK+ installed (if you want to test the GUI once implemented).

### Compilation

1. **Clone the project**:

   ```bash
   git clone https://github.com/Nachiid/TouLaMeche
   ```
2. **Compile the project**:
   If using a Makefile:

   ```bash
   make
   ```

   If compiling manually:

   ```bash
   gcc -o test test.c -lssl -lcrypto
   ```
3. **Run the tests**:

   ```bash
   ./test
   ```

---

## **Project Structure**

The project is organized as follows:

* **test.c**: Contains tests to validate module functionalities.
* **sequence.h**, **sequence.c**: Word sequence (N-gram) management.
* **tableau_dyn.h**, **tableau_dyn.c**: Dynamic array management.
* **arbre_prediction.h**, **arbre_prediction.c**: Prediction tree implementation.
* **entrees_sorties.h**, **entrees_sorties.c** (in progress): User input and file handling.
* **interface_gtk.h**, **interface_gtk.c** (in progress): GUI creation using GTK+.

---

## **Tests**

The tests include:

1. **Sequence Module**: Validation of sequence storage and retrieval features.
2. **Dynamic Array Module**: Resizing and element lookup tests.
3. **Prediction Tree Module**: Accuracy and performance testing for word prediction.
4. **Stress tests**: Robustness verification with large volumes of data.

---

## **License**

```text
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
