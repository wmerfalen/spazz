CC = /home/lmn/gcc-4.9.2/bin/g++

all: arp.o util.o regex.o network.o 
	 $(CC) -std=c++11 -lpthread -Wall -lnet main.cpp arp.o util.o regex.o network.o -o spazz 
util.o:
	$(CC) -std=c++11 -c -Wall util.c -o util.o
arp.o:
	$(CC) -std=c++11 -c -Wall arp.c -o arp.o
regex.o:
	$(CC) -std=c++11 -c -Wall regex.cpp -o regex.o
network.o:
	$(CC) -std=c++11 -c -Wall network.cpp -o network.o
intercept.o:
	$(CC) -std=c++11 -Wall intercept.cpp -o intercept.o
