all: generator.o datownik.o odbiorniki

datownik.o: datownik.c libCommon.so
	gcc -Wl,-rpath=./ -o datownik.o datownik.c -L./ -lCommon

generator.o: generator.c libCommon.so
	gcc -Wl,-rpath=./ -o generator.o generator.c -L./ -lCommon

odbiorniki: skrupulant.o len.o wandal.o

skrupulant.o: skrupulant.c libCommon.so
	gcc -Wl,-rpath=./ -o skrupulant.o skrupulant.c -L./ -lCommon

len.o: len.c libCommon.so
	gcc -Wl,-rpath=./ -o len.o len.c -L./ -lCommon

wandal.o: wandal.c libCommon.so
	gcc -Wl,-rpath=./ -o wandal.o wandal.c -L./ -lCommon

libCommon.so : common.c
	gcc -fPIC -shared -o libCommon.so common.c -lm -lrt

clean: 
	rm *.o *.so
