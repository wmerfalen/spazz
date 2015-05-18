all: util.o 
	gcc -Wall util.o -lnet main.c -o spazz
util.o:
	gcc -c -Wall util.c -o util.o
