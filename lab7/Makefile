CC=gcc
CFLAGS= -Wall

all:
	make compile

compile: main.c chef.c delivery.c
	$(CC) $(CFLAGS) main.c -o main -lm
	$(CC) $(CFLAGS) chef.c -o chef -lm
	$(CC) $(CFLAGS) delivery.c -o delivery -lm

test: compile
	./main 8 8
clean:
	rm -f main *.o *.a chef delivery

