# Options de compilation
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

# Objets nécessaires pour le programme complet
OBJ = main.o sequence.o tableau_dyn.o arbre_prediction.o test.o error.o
# Bibliothèques fournies (en x86_64)
LIBS = list_x86_64.o hash_x86_64.o

# Cible principale
program: $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o program $(LDFLAGS)

# --- Règles de compilation des objets ---

error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c

sequence.o: sequence.c sequence.h error.h hash.h
	$(CC) $(CFLAGS) -c sequence.c

tableau_dyn.o: tableau_dyn.c tableau_dyn.h error.h
	$(CC) $(CFLAGS) -c tableau_dyn.c

arbre_prediction.o: arbre_prediction.c arbre_prediction.h sequence.h tableau_dyn.h
	$(CC) $(CFLAGS) -c arbre_prediction.c

test.o: test.c test.h sequence.h
	$(CC) $(CFLAGS) -c test.c

main.o: main.c sequence.h test.h
	$(CC) $(CFLAGS) -c main.c

# --- Cible spéciale pour TESTER UNIQUEMENT SEQUENCE ---
# Cette cible compile uniquement ce qui est nécessaire pour tester la séquence
test_seq: main.o sequence.o test.o error.o
	$(CC) main.o sequence.o test.o error.o $(LIBS) -o test_seq $(LDFLAGS)
	@echo "Tapez './test_seq' pour lancer les tests du module sequence."

# Nettoyage
clean :
	rm main.o	sequence.o	error.o	tableau_dyn.o	tableau_dyn.h.gch	arbre_prediction.o	arbre_prediction.h.gch	test.o program