CC=gcc
CFLAGS= -Wall

RAPORTNAME = raport.txt
ARGS = 0.00003

all:
	make main

main: main.c
	$(CC) $(CFLAGS) main.c -o main -D TIMING

raport: main
	rm -f $(RAPORTNAME)
	./main $(ARGS) >> $(RAPORTNAME)
