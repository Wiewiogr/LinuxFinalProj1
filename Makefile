all: powielacz.c datownik.c generator.c odbiorca.c

powielacz.o: powielacz.c
	gcc -o powielacz.o powielacz.c

datownik.o: datownik.c
	gcc -o datownik.o datownik.c -lrt -lm

generator.o: generator.c
	gcc -o generator.o generator.c

odbiorca.o: odbiorca.c
	gcc -o odbiorca.o odbiorca.c
