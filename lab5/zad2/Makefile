CC=gcc
CFLAGS= -Wall

all:
	make main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

test: main
	./main kacperklusek@student.agh.edu.pl tutuljakis tresc_jakas_tez


clean:
	rm -f main *.o *.a
