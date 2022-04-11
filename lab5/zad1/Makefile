CC=gcc
CFLAGS= -Wall

all:
	make main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

test: ./main
	./main commands.txt

clean:
	rm -f main *.o *.a
