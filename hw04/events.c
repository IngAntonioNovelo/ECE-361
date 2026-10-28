#include <stdio.h>
#include <string.h>
#include "tally.h"

int main(void)
{
	char line[128];
	int skipped = 0;

	tally_t *t = tally_create();

	/* Checking if the tally is empty*/
	if (t == NULL)
	{
		fprintf(stderr, "Failed to create tally\n");
		return 1;
	}

	while (fgets(line, sizeof(line), stdin) != NULL)
	{
		/* Determine the input length. */
		size_t len = strlen(line);

		/* Remove the newline, if present. */
		int has_newline = (len > 0 && line[len - 1] == '\n');

		if (has_newline)
		{
			line[len - 1] = '\0';
		}

		/* Handle an input line that filled the buffer. */
		if (!has_newline && len == sizeof(line) - 1)
		{
			int ch;

			/* Consume the remainder of the physical line. */
			while ((ch = getchar()) != '\n' && ch != EOF)
			{
				/* Discard the character. */
			}

			/* Count rejected events, but not comments. */
			if (line[0] != '#')
			{
				skipped++;
			}

			continue;
		}

		/* Ignore empty lines and lines with comments */
		if (line[0] == '\0' || line[0] == '#')
		{
				continue;
		}

		/* keep a count of how many lines are skipped by tally_add */

		int result = tally_add(t, line);

		if (result == -1)
		{
			skipped++;
		}


	}

	tally_print(t, stdout);

	printf("names: %d, lines skipped: %d\n", tally_size(t), skipped);

	tally_destroy(t);

	return 0;
}

