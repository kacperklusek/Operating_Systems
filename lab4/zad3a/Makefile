CC=gcc
CFLAGS= -Wall

all:
	make main catcher sender

main: main.c
	$(CC) $(CFLAGS) main.c -o main

catcher: catcher.c
	$(CC) $(FLAGS) catcher.c -o catcher

sender: sender.c
	$(CC) $(FLAGS) sender.c -o sender

test: main catcher sender
	./main SIGRT 1000

clean:
	rm -f main catcher sender $(RAPORTNAME) *.o *.a
