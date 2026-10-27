#include <stdio.h>
#include <string.h>
#include "tally.h"

int main(void)
{
	char line[128];

	while (fgets(line, sizeof(line), stdin) != NULL)
	{
		/* Determine the input length. */
		size_t len = strlen(line);

		/* Remove the newline, if present. */
		if (len > 0 && line[len - 1] == '\n')
		{
			/* TODO: Replace newline with terminator. */
			line[len -1] = '\0';
		}

		/* We'll process the cleaned line later. */
	}

	return 0;
}

