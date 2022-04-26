CC=gcc
CFLAGS= -Wall -Wno-unused-but-set-variable -Wno-unused-variable

FILENAME = results.txt
COPY = copy_clean
COUNT = char_count

RAPORT_NAME = pomiar_zad_1.txt
FROM_FILE = from.txt
TO_FILE = to.txt

all: copy count


copy: copy_clean.c
	$(CC) $(CFLAGS) -o $(COPY) $(COPY).c -D TIMING

count: char_count.c
	$(CC) $(CFLAGS) -o $(COUNT) $(COUNT).c -D TIMING

raport: copy count
	rm $(RAPORT_NAME)
	./$(COPY) $(FROM_FILE) $(TO_FILE) >> $(RAPORT_NAME)
	echo '\n' >> $(RAPORT_NAME)
	./$(COUNT) e $(FROM_FILE) >> $(RAPORT_NAME)


clean:
	rm *.o *.a *.so  $(COPY) $(COUNT)

