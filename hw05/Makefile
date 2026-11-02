
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g -I.

.PHONY: all test clean

all: lock

lock: main.o lock.o
	$(CC) $(CFLAGS) main.o lock.o -o lock

main.o: main.c lock.h
	$(CC) $(CFLAGS) -c main.c -o main.o

lock.o: lock.c lock.h
	$(CC) $(CFLAGS) -c lock.c -o lock.o

tests/test_lock.o: tests/test_lock.c lock.h
	$(CC) $(CFLAGS) -c tests/test_lock.c -o tests/test_lock.o

tests/test_lock: tests/test_lock.o lock.o
	$(CC) $(CFLAGS) tests/test_lock.o lock.o -o tests/test_lock

test: tests/test_lock
	./tests/test_lock

clean:
	rm -f lock main.o lock.o tests/test_lock.o tests/test_lock
