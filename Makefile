CC=g++
FLAGS=-I./ -std=c++0x

all:
	$(CC) -c PosixDaemon.cpp $(FLAGS)
	$(CC) -c server-linux.cpp $(FLAGS)
	$(CC) -c mcs.cpp $(FLAGS)
	$(CC) -c main.cpp $(FLAGS)
	$(CC) -o mcs  main.o mcs.o server-linux.o PosixDaemon.o
	