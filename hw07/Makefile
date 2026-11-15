
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g
CPPFLAGS = -I.
SANFLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer

.PHONY: all test memcheck clean

all: balance

balance: balance.c istack.c istack.h
	$(CC) $(CPPFLAGS) $(CFLAGS) balance.c istack.c -o balance

tests/test_istack: tests/test_istack.c istack.c istack.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_istack.c istack.c -o tests/test_istack

test: all tests/test_istack
	./tests/test_istack
	sh ./regress.sh

memcheck:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) $(SANFLAGS)" test

clean:
	rm -f balance tests/test_istack tests/test_istack_asan
