/* Check bracket balance on standard input and report pairs and nesting depth. */
#include "istack.h"
#include <stdio.h>


/* Map an opening bracket to its matching closer; reject other characters. */
static int expected_close(int opening)
{
	if (opening == '(')
	{
		return ')';
	}

	if (opening == '[')
	{
		return ']';
	}

	if (opening == '{')
	{
		return '}';
	}

	return -1;
}


/* Track unmatched openers and stop at the first bracket or input error. */
int main(void)
{
	/* The stack holds opening brackets in the order they are encountered. */
	istack_t *open = istack_create();
	if (open == NULL)
	{
		fprintf(stderr, "could not create stack\n");
		return 1;
	}

	/* Columns start at 1 when a character is read; newlines reset to zero. */
	size_t line = 1;
	size_t column = 0;
	/* Keep the largest number of opening brackets stored at once. */
	size_t deepest = 0;
	/* Count a pair only after its closing bracket matches the latest opener. */
	size_t pairs = 0;
	/* getchar returns int so EOF can be distinguished from input bytes. */
	int ch;

	while ((ch = getchar()) != EOF)
	{
		/* Move to the next line without treating the newline as a bracket. */
		if (ch == '\n')
		{
			line++;
			column = 0;
			continue;
		}

		column++;

		/* Save each opener so later matching logic can examine the latest one. */
		if (ch == '(' || ch == '[' || ch == '{')
		{
			/* A failed push requires cleanup before leaving the program. */
			if (istack_push(open, ch) != 0)
			{
				fprintf(stderr, "could not grow stack\n");
				istack_destroy(open);
				return 1;
			}

			/* The number of unmatched openers is the current nesting depth. */
			if (istack_count(open) > deepest)
			{
				deepest = istack_count(open);
			}
		}

		
		/* A closer must match the most recent unmatched opener (LIFO order). */
		else if (ch == ')' || ch == ']' || ch == '}')
		{
			int top;

			/* An empty stack means this closer has no corresponding opener. */
			if (istack_peek(open, &top) != 0)
			{
				printf("line %zu, column %zu: found '%c', nothing open\n",
					line, column, ch);

				istack_destroy(open);
				return 1;
			}

			/* Inspect before popping so a mismatch can report the expected closer. */
			int expected = expected_close(top);

			if (ch != expected)
			{
				printf("line %zu, column %zu: found '%c', expected '%c'\n",
					line, column, ch, expected);

				istack_destroy(open);
				return 1;
			}

			/* Discard the matched opener; its value is no longer needed. */
			if (istack_pop(open, NULL) != 0)
			{
				fprintf(stderr, "could not pop stack\n");
				istack_destroy(open);
				return 1;
			}

			pairs++;
		}


	}

	/* Distinguish a failed read from reaching the normal end of input. */
	if (ferror(stdin))
{
	fprintf(stderr, "could not read input\n");
	istack_destroy(open);
	return 1;
}

/* At EOF, any remaining opener is missing its closing bracket. */
if (!istack_is_empty(open))
{
	int top;

	if (istack_peek(open, &top) != 0)
	{
		fprintf(stderr, "could not inspect stack\n");
		istack_destroy(open);
		return 1;
	}

	/* The stack top identifies the innermost bracket still awaiting a closer. */
	printf("end of input: %zu still open, innermost '%c'\n",
		istack_count(open), top);

	istack_destroy(open);
	return 1;
}

/* No mismatches or leftover openers remain, so report the completed totals. */
printf("balanced: %zu pairs, deepest %zu\n",
	pairs, deepest);

/* Release the stack before returning success. */
istack_destroy(open);
return 0;

	/* Free both the stack's array and its structure after processing input. */
	istack_destroy(open);
	return 0;
}
