CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g -I.
SANFLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer -O0

.PHONY: all test memcheck clean
all: treestats tests/test_bst

treestats: treestats.c bst.c bst.h
	$(CC) $(CFLAGS) bst.c treestats.c -o $@

tests/test_bst: tests/test_bst.c bst.c bst.h
	$(CC) $(CFLAGS) bst.c tests/test_bst.c -o $@

tests/test_bst_asan: tests/test_bst.c bst.c bst.h
	$(CC) $(CFLAGS) $(SANFLAGS) bst.c tests/test_bst.c -o $@

test: all
	./tests/test_bst
	printf '27\n14\n35\n10\n19\n31\n42\n' | ./treestats | grep -Fx 'n 7, height 3'
	printf '' | ./treestats | grep -Fx 'empty tree'
	@set +e; printf '1\n2\n3\n4\noops\n' | ./treestats > tests/invalid_output.txt; status=$$?; set -e; \
		grep -Fx 'not a number after 4 values' tests/invalid_output.txt && test $$status -eq 1

memcheck: tests/test_bst_asan
	ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 ./tests/test_bst_asan

clean:
	rm -f treestats tests/test_bst tests/test_bst_asan tests/invalid_output.txt tests/test_remove_bonus *.o

.PHONY: bonus
bonus: tests/test_remove_bonus
	ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 ./tests/test_remove_bonus

tests/test_remove_bonus: tests/test_remove.c bst.c bst.h
	$(CC) $(CFLAGS) $(SANFLAGS) bst.c tests/test_remove.c -o $@
