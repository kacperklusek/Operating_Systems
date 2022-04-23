CC=gcc
CFLAGS= -Wall

all:
	make main
	rm -f log.txt

main: server.c client.c
	$(CC) $(CFLAGS) server.c -o server
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f main *.o *.a log.txt server client

