CC=gcc
CFLAGS= -Wall

RAPORTNAME = raport.txt
ARGS = ./tree something 5

all:
	make main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

raport: main
	rm -f $(RAPORTNAME)
	./main $(ARGS) >> $(RAPORTNAME)

clean:
	rm -f main $(RAPORTNAME) *.o *.a