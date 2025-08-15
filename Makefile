CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Werror -O2

all: demo

byte_queue.o: byte_queue.c byte_queue.h
	$(CC) $(CFLAGS) -c byte_queue.c -o byte_queue.o

main.o: main.c byte_queue.h
	$(CC) $(CFLAGS) -c main.c -o main.o

demo: byte_queue.o main.o
	$(CC) $(CFLAGS) byte_queue.o main.o -o demo

clean:
	rm -f *.o demo

.PHONY: all clean