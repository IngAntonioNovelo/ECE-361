
#include <stdio.h>
#include "lock.h"

/* Check whether lock_event returns the expected event. */
static int check_event(const char *description,
	state_t state, char input, int expected)
{
	int actual = lock_event(state, input);

	if (actual == expected)
	{
		printf("PASS  %s\n", description);
		return 0;
	}

	printf("FAIL  %s: expected %d, got %d\n",
		description, expected, actual);
	return 1;
}

int main(void)
{
	int failed = 0;

	failed += check_event("correct third digit", GOT2, '4', E_RIGHT);
	failed += check_event("wrong second digit", GOT1, '4', E_WRONG);
	failed += check_event("digit while open", OPEN, '7', E_WRONG);
	failed += check_event("invalid character", LOCKED, 'x', -1);

	printf("Summary: %d passed, %d failed\n", 4 - failed, failed);

	return failed == 0 ? 0 : 1;
}
