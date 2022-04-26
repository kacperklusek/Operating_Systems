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

raport: main
	rm -f $(RAPORTNAME)

	for arg in $(ARG1) $(ARG2) $(ARG3) $(ARG4) ; do\
		printf "$$arg \n" >> $(RAPORTNAME) ;\
		./main $$arg >> $(RAPORTNAME) ;\
		printf "\n" >> $(RAPORTNAME) ;\
	done ;\

clean:
	rm -f main $(RAPORTNAME) *.o *.a
