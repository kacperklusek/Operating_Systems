CC=gcc
CFLAGS= -Wall

all:
	make main consumer producer

main: main.c
	$(CC) $(CFLAGS) main.c -o main

consumer: consumer.c
	$(CC) $(CFLAGS) consumer.c -o consumer

producer: producer.c
	$(CC) $(CFLAGS) producer.c -o producer

test: main consumer producer
	rm -f fifo_pipe
	./main

clean:
	rm -f main fifo_pipe out_file.txt producer consumer *.o *.a
