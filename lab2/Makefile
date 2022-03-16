CC=gcc
CFLAGS= -Wall

FILENAME = results.txt
COPY = copy_clean
COUNT = char_count

all: copy count


copy: copy_clean.c
	$(CC) $(CFLAGS) -o $(COPY) $(COPY).c

count: char_count.c
	$(CC) $(CFLAGS) -o $(COUNT) $(COUNT).c

clean:
	rm *.o *.a *.so  $(COPY) $(COUNT)

