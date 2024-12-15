program : main.o sequence.o	tableau_dyn.o	arbre_prediction.o	test.o	
	gcc	main.o	sequence.o	test.o	tableau_dyn.o	arbre_prediction.o	list_x86_64.o	hash_x86_64.o	-o	program	-lm

main.o : main.c sequence.h
	gcc -c main.c -o main.o

sequence.o : sequence.c
	gcc -c sequence.c -o sequence.o

tableau_dyn.o	:	tableau_dyn.c	tableau_dyn.h
	gcc	-c	tableau_dyn.c	tableau_dyn.h

arbre_prediction.o	:	arbre_prediction.c	arbre_prediction.h
	gcc	-c	arbre_prediction.c	arbre_prediction.h

test.o	:	test.c	test.h
	gcc	-c	test.c	-o	test.o


clean :
	rm main.o	sequence.o	tableau_dyn.o	tableau_dyn.h.gch	arbre_prediction.o	arbre_prediction.h.gch	test.o program