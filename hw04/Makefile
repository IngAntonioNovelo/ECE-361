
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g -I.

.PHONY: all test clean

all: events

events: events.c tally.c tally.h
	$(CC) $(CFLAGS) events.c tally.c -o events

test:
	bash run_hw04_tests.sh

clean:
	rm -f events tally.o tests/test_hw04_tally
