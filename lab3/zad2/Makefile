CC=gcc
CFLAGS= -Wall

RAPORT_NAME = raport.txt
ARGS = 0.00003

all:
	make main

main: main.c
	$(CC) $(CFLAGS) main.c -o main -D TIMING

raport: main
	rm -f $(RAPORTNAME)
	./main $(ARGS) >> $(RAPORT_NAME)

clean:
	rm -f *.o *.a main $(RAPORT_NAME) res/*