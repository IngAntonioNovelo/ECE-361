#include <stdio.h>
#include "lock.h"

/* Read characters, apply recognized events, and print each transition. */
int main(void)
{
	state_t current_state = LOCKED;
	int input;

	while ((input = getchar()) != EOF)
	{
		int event = lock_event(current_state, (char)input);

		/* Ignore whitespace and other unrecognized characters. */
		if (event == -1)
		{
			continue;
		}

		action_t action;
		state_t next_state = lock_step(current_state, (event_t)event, &action);

		printf("%s %c %s -> %s %s\n",
			state_name(current_state),
			(char)input,
			event_name((event_t)event),
			state_name(next_state),
			action_name(action));

		current_state = next_state;
	}

	if (ferror(stdin))
	{
		fputs("Error: failed to read standard input.\n", stderr);
		return 1;
	}

	printf("final: %s\n", state_name(current_state));
	return 0;
}
