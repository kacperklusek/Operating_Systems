CC=gcc
CFLAGS= -Wall -Wno-unused-but-set-variable
PICFLAGS=
SHAREDFLAGS=-shared
AR = ar
RCS = rcs

FILENAME = results3b.txt

all: static

# build shared library
shared: library.c main.c
	$(CC) -c -fPIC library.c
	$(CC) -shared -fPIC -o library.so library.o
	$(CC) $(CFLAGS) ./main.c -L. -llibrary -D -I. TIMING -o main

# build static library
static: main.c library.c
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) -c library.c
	$(AR) $(RCS) liblibrary.a library.o
	#$(CC) main.o liblibrary.a -o main
	$(CC) $(CFLAGS) ./main.c  -L. -llibrary -D TIMING -o main

dynamic:
	make -C ./ shared
	$(CC) $(CFLAGS) ./main.c -ldl -D LIB_DYNAMIC -D TIMING -o main


test:
	rm -f $(FILENAME)

	for imprt in static shared ; do \
  		for opt in O0 O1 O2 O3 Ofast Os ; do \
  			make $$imprt -$$opt ; \
  			printf "$$imprt $$opt\n" >> $(FILENAME) ; \
  			make raport ; \
  			printf "\n" >> $(FILENAME) ; \
  		done ; \
  	done ; \

raport:
	@printf "One file:\n"  2>/dev/null >> $(FILENAME)
	./main `cat ./test_files/com1.txt | tr '\n' ' '`  2>/dev/null >> $(FILENAME)

	@printf "\nallocate 100 blocks, wc 10 big_files:\n"  >> $(FILENAME)
	./main `cat ./test_files/com2.txt | tr '\n' ' '` 2>/dev/null >> $(FILENAME)

	@printf "\nallocate 1500 blocks, wc many different files:\n"  >> $(FILENAME)
	./main `cat ./test_files/com3.txt | tr '\n' ' '` 2>/dev/null >> $(FILENAME)

	@printf "\nallocate 1500 blocks, wc loads of files:\n"  >> $(FILENAME)
	./main `cat ./test_files/com4.txt | tr '\n' ' '` 2>/dev/null >> $(FILENAME)

clean:
	rm *.o *.a *.so *static main

