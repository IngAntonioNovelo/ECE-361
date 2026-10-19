ECE 361 — Homework 03: Pointer Library
Overview
This homework implements a small C library for working with floating-point values and arrays using pointers. The implementations in ptr.c use pointer arithmetic instead of array indexing.
Functions
- void swap_float(float *a, float *b) — Exchanges the values pointed to by a and b. The caller must provide valid, writable float pointers.
- int minmax(const float *a, int n, float *min, float *max) — Finds the smallest and largest of n floats and writes them through the output pointers. Returns 0 on success. Returns -1, without modifying the outputs, if n < 1 or any pointer is NULL.
- const float *find_above(const float *a, int n, float threshold) — Returns a pointer to the first array element strictly greater than threshold, or NULL when no match is found.
- void reverse(float *a, int n) — Reverses an array in place using two pointers and swap_float(). Does nothing when a is NULL or n < 2.
Project Files
- ptr.h — Function declarations and interface documentation.
- ptr.c — Function implementations.
- tests/test_ptr.c — Tests for the four pointer functions.
- Makefile — Build, test, and cleanup commands.
- README.md — Build and usage information.
- AI_USAGE.md — Description of AI assistance.
Requirements
GCC and make in a Linux environment (including WSL).
Build and Test
Run the following commands from the hw03 directory:
make
make test
make clean
- make compiles ptr.o and creates tests/test_ptr.
- make test builds (if needed) and runs the tests, printing PASS/FAIL results and a summary.
- make clean removes the generated object file and test executable.
For an additional memory-safety check, run:
make clean
make CFLAGS="-std=c11 -Wall -Wextra -g -fsanitize=address,undefined" test
The goal is a warning-free build with all tests passing. Array indexing is permitted in the test file, but not in the implementations in ptr.c.
