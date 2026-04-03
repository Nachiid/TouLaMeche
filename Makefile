########################################################################
# Projet : TouLaMeche
# Description : Makefile généraliste pour compilation du projet
########################################################################

# Compilateur
CC := gcc

# Flags de compilation
CFLAGS := -Wall -Wextra -Werror -std=c11 -g

# Nom de l'exécutable
TARGET := main

# --- CORRECTION 1 : On exclut les fichiers de test du wildcard ---
SRC := $(filter-out test_hash_exemple.c, $(wildcard *.c))

# Objets générés automatiquement
OBJ := $(SRC:.c=.o)

# Objets externes (NE PAS SUPPRIMER)
EXT_OBJ := hash_x86_64.o list_x86_64.o

# Dépendances automatiques
DEP := $(OBJ:.o=.d)

########################################################################
# Règles principales
########################################################################

all: $(TARGET)

# --- CORRECTION 2 : Ajout de -lm à la fin pour sqrt ---
$(TARGET): $(OBJ) $(EXT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

########################################################################
# Compilation automatique
########################################################################

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

########################################################################
# Exécution
########################################################################

run: $(TARGET)
	./$(TARGET)

########################################################################
# Nettoyage (NON TOUCHÉ)
########################################################################

clean:
	rm -f $(OBJ) $(DEP)

fclean: clean
	rm -f $(TARGET)

re: fclean all

########################################################################
# Inclusion des dépendances
########################################################################

-include $(DEP)

.PHONY: all run clean fclean re