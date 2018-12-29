utils: types.o utils.o
	clang -std=c11 -o bin utils.o types.o

types.o: types.c
	clang -std=c11 -o types.o -c types.c -W -Wall -ansi -pedantic

utils.o: utils.c types.h
	clang -std=c11 -o utils.o -c utils.c -W -Wall -ansi -pedantic
