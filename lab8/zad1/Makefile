CC=gcc
CFLAGS= -Wall
RAPORT_NAME = Times.txt


all:
	make main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

test: main
	#./main 3 numbers ./images/baboon.pgm inverted_baboon.pgm
	rm -f $(RAPORT_NAME)
	for n_of_threads in 1 2 4 8 16; do \
		for mode in numbers block; do \
		  	rm -f inverted_baboon ; \
			printf "Inverting image with $$n_of_threads threads using mode: $$mode \n" >> $(RAPORT_NAME) ; \
			./main $$n_of_threads $$mode ./images/baboon.pgm inverted_baboon.pgm >> $(RAPORT_NAME) ; \
			printf "\n\n" $(RAPORT_NAME) ; \
		done ; \
	done ; \

clean:
	rm -f main *.o *.a $(RAPORT_NAME) inverted_baboon_*
