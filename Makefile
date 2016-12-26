all: powielacz.c datownik.c generator.c odbiornik.c

powielacz.o: powielacz.c
	gcc -o powielacz.o powielacz.c

datownik.o: datownik.c libCommon.so
	gcc -Wl,-rpath=./ -o datownik.o datownik.c -L./ -lCommon

generator.o: generator.c
	gcc -o generator.o generator.c

odbiornik.o: odbiornik.c libCommon.so
	gcc -Wl,-rpath=./ -o odbiornik.o odbiornik.c -L./ -lCommon

skrupulant.o: skrupulant.c libCommon.so
	gcc -Wl,-rpath=./ -o skrupulant.o skrupulant.c -L./ -lCommon

len.o: len.c libCommon.so
	gcc -Wl,-rpath=./ -o len.o len.c -L./ -lCommon

wandal.o: wandal.c libCommon.so
	gcc -Wl,-rpath=./ -o wandal.o wandal.c -L./ -lCommon

libCommon.so : common.c
	gcc -shared -o libCommon.so common.c -lm -lrt

clean: 
	rm *.o *.so
