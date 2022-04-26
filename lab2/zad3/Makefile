CC=gcc
CFLAGS= -Wall -Wno-unused-but-set-variable -Wno-unused-variable

FILENAME = results.txt
DIRNAME = tree
PROGNAME = main

RAPORT_NAME = raport3.txt

all: main

main: main.c
	$(CC) $(CFLAGS) -o $(PROGNAME) $(PROGNAME).c

raport: main
	rm -f $(RAPORT_NAME)
	./$(PROGNAME) $(DIRNAME) >> $(RAPORT_NAME)

clean:
	rm *.o *.a *.so $(PROGNAME)

