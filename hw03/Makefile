
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g

all: ptr.o tests/test_ptr

ptr.o: ptr.c ptr.h
	$(CC) $(CFLAGS) -c ptr.c -o ptr.o

tests/test_ptr: tests/test_ptr.c ptr.o ptr.h
	$(CC) $(CFLAGS) -I. tests/test_ptr.c ptr.o -o tests/test_ptr

test: all
	./tests/test_ptr

clean:
	rm -f ptr.o tests/test_ptr

.PHONY: all test clean
