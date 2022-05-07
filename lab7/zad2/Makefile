CC=gcc
CFLAGS= -Wall
LIBS=-lm -lrt -lpthread

all:
	make compile

compile: main.c chef.c delivery.c
	$(CC) $(CFLAGS) main.c -o main $(LIBS)
	$(CC) $(CFLAGS) chef.c -o chef $(LIBS)
	$(CC) $(CFLAGS) delivery.c -o delivery $(LIBS)

test: compile
	./main 8 8
clean:
	rm -f main *.o *.a chef delivery

