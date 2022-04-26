CC=gcc
CFLAGS= -Wall

RAPORTNAME = raport2.txt
ARG1 = ignore
ARG2 = handler
ARG3 = mask
ARG4 = pending

all:
	make main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

test: main
	./main

clean:
	rm -f main $(RAPORTNAME) *.o *.a
