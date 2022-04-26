CC=gcc
CFLAGS= -Wall
PICFLAGS=
SHAREDFLAGS=-shared
AR = ar
RCS = rcs

FILENAME2 = results2.txt

all: static

# build shared library
shared: library.c
	$(CC) -Wall -fPIC -c library.c
	$(CC) -shared -fPIC -o library.so library.o

# build static library
static: main.o liblibrary.a
	$(CC) main.o liblibrary.a -o main

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

liblibrary.a: library.o
	$(AR) $(RCS) liblibrary.a library.o

library.o: library.c
	$(CC) $(CFLAGS) -c library.c

# TODO build dynamic library

# run tests
test_wc:
	rm -f $(FILENAME2)

	@printf "One file:\n"  2>/dev/null >> $(FILENAME2)
	./main `cat ./test_files/com1.txt | tr '\n' ' '`  2>/dev/null >> $(FILENAME2)

	@printf "\nallocate 100 blocks, wc 10 big_files:\n"  >> $(FILENAME2)
	./main `cat ./test_files/com2.txt | tr '\n' ' '` 2>/dev/null >> $(FILENAME2)

	@printf "\nallocate 1500 blocks, wc many different files:\n"  >> $(FILENAME2)
	./main `cat ./test_files/com3.txt | tr '\n' ' '` 2>/dev/null >> $(FILENAME2)

	@printf "\nallocate 1500 blocks, wc loads of files:\n"  >> $(FILENAME2)
	./main `cat ./test_files/com4.txt | tr '\n' ' '` 2>/dev/null >> $(FILENAME2)


clean:
	rm *.o *.a main

