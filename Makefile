all: powielacz.c datownik.c generator.c odbiorca.c

powielacz.o: powielacz.c
	gcc -o powielacz.o powielacz.c

datownik.o: datownik.c libCommon.so
	gcc -Wl,-rpath=./ -o datownik.o datownik.c -lrt -L./ -lCommon -lm 

generator.o: generator.c
	gcc -o generator.o generator.c

odbiorca.o: odbiorca.c
	gcc -o odbiorca.o odbiorca.c

libCommon.so : libCommon.c
	gcc -shared -o libCommon.so libCommon.c -lm
