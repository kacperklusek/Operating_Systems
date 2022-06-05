CC=gcc
CFLAGS= -Wall


all:
	make main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

test: main
	./main 5

clean:
	rm -f main *.o *.a
