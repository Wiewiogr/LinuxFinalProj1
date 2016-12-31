COMMON_FLAGS = -Wl,-rpath=./ -L./ -lCommon

all: generator.out datownik.out powielacz.out odbiorniki

powielacz.out: powielacz.c libCommon.so
	gcc -o powielacz.out powielacz.c $(COMMON_FLAGS)

datownik.out: datownik.c libCommon.so
	gcc -o datownik.out datownik.c $(COMMON_FLAGS)

generator.out: generator.c libCommon.so
	gcc -o generator.out generator.c $(COMMON_FLAGS)

odbiorniki: skrupulant.out len.out wandal.out

skrupulant.out: skrupulant.c libCommon.so
	gcc -o skrupulant.out skrupulant.c $(COMMON_FLAGS)

len.out: len.c libCommon.so
	gcc -o len.out len.c $(COMMON_FLAGS)

wandal.out: wandal.c libCommon.so
	gcc -o wandal.out wandal.c $(COMMON_FLAGS)

libCommon.so : common.c
	gcc -fPIC -shared -o libCommon.so common.c -lm -lrt

clean: 
	rm *.out *.so
