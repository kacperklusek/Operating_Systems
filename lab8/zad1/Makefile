CC=gcc
CFLAGS= -Wall

all:
	make main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

test: main
	./main 4 1 ./images/baboon.pgm inverted_baboon.pgm

clean:
	rm -f main *.o *.a
