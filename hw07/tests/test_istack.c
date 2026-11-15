
#include "istack.h"
#include <stdio.h>
#include <stdlib.h>

/* Track assertion results for the final summary and exit status. */
static int passed = 0;
static int failed = 0;

/* Print a named result and record whether its condition succeeded. */
static void check(int ok, const char *name)
{
	printf("%s  %s\n", ok ? "PASS" : "FAIL", name);

	if (ok)
	{
		passed++;
	}
	else
	{
		failed++;
	}
}

/* Invalid operations must fail without overwriting the output value. */
static void test_null_operations(void)
{
	int value = 123;

	check(istack_push(NULL, 1) == -1, "NULL push");
	check(istack_pop(NULL, &value) == -1 && value == 123,
		"NULL pop preserves output");
	check(istack_peek(NULL, &value) == -1 && value == 123,
		"NULL peek preserves output");
	istack_destroy(NULL);
}

/* Popping or peeking an empty stack must leave its state unchanged. */
static void test_empty_operations(istack_t *s)
{
	int value = 123;

	check(istack_pop(s, &value) == -1 && value == 123,
		"empty pop preserves output");
	check(istack_peek(s, &value) == -1 && value == 123,
		"empty peek preserves output");
	check(istack_pop(s, NULL) == -1 &&
		istack_peek(s, NULL) == -1 && istack_count(s) == 0,
		"empty operations accept NULL outputs safely");
}

/* Check LIFO order, optional outputs, and reuse after removing all values. */
static void test_stack_operations(istack_t *s)
{
	check(istack_push(s, -12) == 0 && istack_push(s, 0) == 0,
		"push negative and zero values");

	int value = 123;
	check(istack_peek(s, &value) == 0 && value == 0 &&
		istack_count(s) == 2, "peek preserves count");
	check(istack_peek(s, NULL) == 0 && istack_count(s) == 2,
		"peek allows NULL output");
	check(istack_pop(s, &value) == 0 && value == 0 &&
		istack_count(s) == 1, "pop returns most recent value");
	check(istack_peek(s, &value) == 0 && value == -12,
		"pop exposes previous value");
	check(istack_pop(s, NULL) == 0 && istack_is_empty(s),
		"pop discards value with NULL output");
	test_empty_operations(s);
	check(istack_push(s, 42) == 0 &&
		istack_pop(s, &value) == 0 && value == 42 &&
		istack_is_empty(s), "reuse drained stack");
}

/* The header permits copying without requesting the array length. */
static void test_copy_without_length(istack_t *s)
{
	int *copy = istack_to_array(s, NULL);

	check(copy != NULL && copy[0] == 10 && copy[8] == 90,
		"to_array allows NULL length");
	free(copy);
}

int main(void)
{
	test_null_operations();
	/* NULL stacks should report zero count, zero capacity, and empty status. */
	check(istack_count(NULL) == 0 &&
		istack_is_empty(NULL) &&
		istack_capacity(NULL) == 0,
		"NULL metadata");

	/* A nonzero starting length verifies that to_array resets it on failure. */
	size_t n = 99;
	int *a = istack_to_array(NULL, &n);

	check(a == NULL && n == 0,
		"NULL to_array");

	/* Stop if allocation fails because the remaining tests need a stack. */
	istack_t *s = istack_create();

	if (s == NULL)
	{
		puts("FAIL  create stack");
		return 1;
	}

	/* A new stack starts empty with space for eight integers. */
	check(istack_count(s) == 0 &&
		istack_is_empty(s) &&
		istack_capacity(s) == 8,
		"new stack metadata");

	/* An empty stack produces no array and reports a zero length. */
	n = 99;
	a = istack_to_array(s, &n);

	check(a == NULL && n == 0,
		"empty to_array");
	free(a);

	test_empty_operations(s);
	test_stack_operations(s);

	/* The ninth push exceeds the initial capacity and should double it. */
	int pushed = 1;

	for (int i = 1; i <= 9; i++)
	{
		if (istack_push(s, i * 10) != 0)
		{
			pushed = 0;
			break;
		}
	}

	check(pushed &&
		istack_count(s) == 9 &&
		istack_capacity(s) == 16 &&
		!istack_is_empty(s),
		"nine pushes double capacity");
	if (pushed)
	{
		test_copy_without_length(s);
	}

	/* Verify that the exported values retain insertion order, bottom first. */
	n = 0;
	a = istack_to_array(s, &n);

	int copied = a != NULL && n == 9;

	if (copied)
	{
		for (size_t i = 0; i < n; i++)
		{
			if (a[i] != (int)(i + 1) * 10)
			{
				copied = 0;
			}
		}
	}

	check(copied, "to_array is bottom-first");

	/* Change the copied bottom value to detect shared storage with the stack. */
	if (a != NULL)
	{
		a[0] = 777;
	}

	int top = 0;

	int independent = istack_peek(s, &top) == 0 &&
		top == 90;

	/* Pop in reverse insertion order, leaving the original bottom value. */
	for (int i = 90; i >= 20; i -= 10)
	{
		int value = 0;

		if (istack_pop(s, &value) != 0 ||
			value != i)
		{
			independent = 0;
		}
	}

	int value = -1;

	/* The stack's bottom must still be 10 despite changing the copy to 777. */
	check(independent &&
		istack_peek(s, &value) == 0 &&
		value == 10,
		"copy is independent of stack");

	/* The caller owns the exported array separately from the stack. */
	free(a);
	istack_destroy(s);

	printf("Summary: %d passed, %d failed\n",
		passed, failed);

	/* Return a failing process status if any recorded check failed. */
	return failed == 0 ? 0 : 1;
}
