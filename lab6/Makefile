CC=gcc
CFLAGS= -Wall

all:
	make main

main: server.c client.c
	$(CC) $(CFLAGS) server.c -o server
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f main *.o *.a

