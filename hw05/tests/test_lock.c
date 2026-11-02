#include <stdio.h>
#include <string.h>
#include "lock.h"

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

static int passed = 0;
static int failed = 0;

/* Record one result and print PASS or FAIL. */
static void report(const char *name, int ok)
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

/* Check both the public table and lock_step for every state/event pair. */
static void test_transitions(void)
{
	static const transition_t expected[N_STATES][N_EVENTS] = {
		[LOCKED] = {
			[E_RIGHT] = { GOT1, A_NONE },
			[E_WRONG] = { LOCKED, A_BEEP },
			[E_CLOSE] = { LOCKED, A_NONE },
			[E_TIMEOUT] = { LOCKED, A_NONE }
		},
		[GOT1] = {
			[E_RIGHT] = { GOT2, A_NONE },
			[E_WRONG] = { LOCKED, A_BEEP },
			[E_CLOSE] = { GOT1, A_NONE },
			[E_TIMEOUT] = { LOCKED, A_NONE }
		},
		[GOT2] = {
			[E_RIGHT] = { OPEN, A_UNLOCK },
			[E_WRONG] = { LOCKED, A_BEEP },
			[E_CLOSE] = { GOT2, A_NONE },
			[E_TIMEOUT] = { LOCKED, A_NONE }
		},
		[OPEN] = {
			[E_RIGHT] = { OPEN, A_NONE },
			[E_WRONG] = { OPEN, A_NONE },
			[E_CLOSE] = { LOCKED, A_LOCK },
			[E_TIMEOUT] = { LOCKED, A_LOCK }
		}
	};

	for (int s = 0; s < N_STATES; s++)
	{
		for (int e = 0; e < N_EVENTS; e++)
		{
			const transition_t want = expected[s][e];
			const transition_t actual = lock_table[s][e];
			action_t action = A_UNDEFINED;
			state_t next = lock_step((state_t)s, (event_t)e, &action);
			char label[96];
			int ok = actual.next == want.next &&
				actual.action == want.action &&
				next == want.next && action == want.action;

			snprintf(label, sizeof(label), "transition %s + %s",
				state_name((state_t)s), event_name((event_t)e));
			report(label, ok);
			if (!ok)
			{
				printf("      expected: %s / %s; table: %s / %s; step: %s / %s\n",
					state_name(want.next), action_name(want.action),
					state_name(actual.next), action_name(actual.action),
					state_name(next), action_name(action));
			}
		}
	}
}

/* Check character-to-event conversion independently of state transitions. */
static void test_events(void)
{
	static const struct {
		const char *name;
		state_t state;
		char input;
		int expected;
	} cases[] = {
		{ "LOCKED: correct 3", LOCKED, '3', E_RIGHT },
		{ "LOCKED: wrong 1", LOCKED, '1', E_WRONG },
		{ "GOT1: correct 1", GOT1, '1', E_RIGHT },
		{ "GOT1: wrong 3", GOT1, '3', E_WRONG },
		{ "GOT2: correct 4", GOT2, '4', E_RIGHT },
		{ "GOT2: wrong 1", GOT2, '1', E_WRONG },
		{ "OPEN: digit 3", OPEN, '3', E_WRONG },
		{ "OPEN: digit 1", OPEN, '1', E_WRONG },
		{ "OPEN: digit 4", OPEN, '4', E_WRONG },
		{ "OPEN: digit 7", OPEN, '7', E_WRONG },
		{ "LOCKED: close", LOCKED, 'c', E_CLOSE },
		{ "GOT1: close", GOT1, 'c', E_CLOSE },
		{ "GOT2: close", GOT2, 'c', E_CLOSE },
		{ "OPEN: close", OPEN, 'c', E_CLOSE },
		{ "LOCKED: timeout", LOCKED, 't', E_TIMEOUT },
		{ "GOT1: timeout", GOT1, 't', E_TIMEOUT },
		{ "GOT2: timeout", GOT2, 't', E_TIMEOUT },
		{ "OPEN: timeout", OPEN, 't', E_TIMEOUT },
		{ "invalid: letter x", LOCKED, 'x', -1 },
		{ "invalid: newline", GOT1, '\n', -1 },
		{ "invalid: hash", OPEN, '#', -1 },
		{ "invalid: uppercase C", LOCKED, 'C', -1 }
	};

	for (size_t i = 0; i < ARRAY_LEN(cases); i++)
	{
		int actual = lock_event(cases[i].state, cases[i].input);
		report(cases[i].name, actual == cases[i].expected);
		if (actual != cases[i].expected)
		{
			printf("      expected event %d, got %d\n",
				cases[i].expected, actual);
		}
	}
}

/* Simulate input from LOCKED and verify every intermediate result. */
static void check_sequence(const char *name, const char *input,
	const state_t *expected_states, const action_t *expected_actions,
	size_t expected_steps)
{
	state_t state = LOCKED;
	size_t step = 0;
	int ok = 1;

	for (const char *p = input; *p != '\0'; p++)
	{
		int event = lock_event(state, *p);
		if (event == -1)
		{
			continue;
		}
		if (step >= expected_steps)
		{
			printf("      unexpected extra event at input '%c'\n", *p);
			ok = 0;
			break;
		}

		action_t action = A_UNDEFINED;
		state = lock_step(state, (event_t)event, &action);
		if (state != expected_states[step] || action != expected_actions[step])
		{
			printf("      step %zu (input '%c'): expected %s / %s, got %s / %s\n",
				step + 1, *p,
				state_name(expected_states[step]), action_name(expected_actions[step]),
				state_name(state), action_name(action));
			ok = 0;
		}
		step++;
	}

	if (step != expected_steps)
	{
		printf("      expected %zu processed events, got %zu\n", expected_steps, step);
		ok = 0;
	}
	report(name, ok);
}

/* Required sequences plus additional regression cases. */
static void test_sequences(void)
{
	static const state_t s314[] = { GOT1, GOT2, OPEN };
	static const action_t a314[] = { A_NONE, A_NONE, A_UNLOCK };
	static const state_t s3314[] = { GOT1, LOCKED, LOCKED, LOCKED };
	static const action_t a3314[] = { A_NONE, A_BEEP, A_BEEP, A_BEEP };
	static const state_t s31t4[] = { GOT1, GOT2, LOCKED, LOCKED };
	static const action_t a31t4[] = { A_NONE, A_NONE, A_NONE, A_BEEP };
	static const state_t s314c[] = { GOT1, GOT2, OPEN, LOCKED };
	static const action_t a314c[] = { A_NONE, A_NONE, A_UNLOCK, A_LOCK };
	static const state_t s3c14[] = { GOT1, GOT1, GOT2, OPEN };
	static const action_t a3c14[] = { A_NONE, A_NONE, A_NONE, A_UNLOCK };
	static const state_t s31c4[] = { GOT1, GOT2, GOT2, OPEN };
	static const action_t a31c4[] = { A_NONE, A_NONE, A_NONE, A_UNLOCK };
	static const state_t s3147[] = { GOT1, GOT2, OPEN, OPEN };
	static const action_t a3147[] = { A_NONE, A_NONE, A_UNLOCK, A_NONE };
	static const state_t s314t[] = { GOT1, GOT2, OPEN, LOCKED };
	static const action_t a314t[] = { A_NONE, A_NONE, A_UNLOCK, A_LOCK };

	check_sequence("sequence 314 opens", "314", s314, a314, ARRAY_LEN(s314));
	check_sequence("sequence 3314 does not open", "3314", s3314, a3314, ARRAY_LEN(s3314));
	check_sequence("sequence 31t4 does not open", "31t4", s31t4, a31t4, ARRAY_LEN(s31t4));
	check_sequence("sequence 314c locks again", "314c", s314c, a314c, ARRAY_LEN(s314c));
	check_sequence("sequence 3c14 preserves GOT1", "3c14", s3c14, a3c14, ARRAY_LEN(s3c14));
	check_sequence("sequence 31c4 preserves GOT2", "31c4", s31c4, a31c4, ARRAY_LEN(s31c4));
	check_sequence("sequence 3147 ignores digit while open", "3147", s3147, a3147, ARRAY_LEN(s3147));
	check_sequence("sequence 314t locks on timeout", "314t", s314t, a314t, ARRAY_LEN(s314t));
	check_sequence("sequence ignores unrecognized chars", "3\n1#4", s314, a314, ARRAY_LEN(s314));
}

/* Verify the printable names and bounds checks. */
static void test_names(void)
{
	static const char *const states[] = { "LOCKED", "GOT1", "GOT2", "OPEN" };
	static const char *const events[] = { "RIGHT", "WRONG", "CLOSE", "TIMEOUT" };
	static const char *const actions[] = { "UNDEFINED", "NONE", "UNLOCK", "LOCK", "BEEP" };
	char label[80];

	for (int s = 0; s < N_STATES; s++)
	{
		snprintf(label, sizeof(label), "state_name(%d)", s);
		report(label, strcmp(state_name((state_t)s), states[s]) == 0);
	}
	for (int e = 0; e < N_EVENTS; e++)
	{
		snprintf(label, sizeof(label), "event_name(%d)", e);
		report(label, strcmp(event_name((event_t)e), events[e]) == 0);
	}
	for (int a = 0; a < N_ACTIONS; a++)
	{
		snprintf(label, sizeof(label), "action_name(%d)", a);
		report(label, strcmp(action_name((action_t)a), actions[a]) == 0);
	}

	report("state_name rejects negative", strcmp(state_name((state_t)-1), "?") == 0);
	report("state_name rejects N_STATES", strcmp(state_name(N_STATES), "?") == 0);
	report("event_name rejects negative", strcmp(event_name((event_t)-1), "?") == 0);
	report("event_name rejects N_EVENTS", strcmp(event_name(N_EVENTS), "?") == 0);
	report("action_name rejects negative", strcmp(action_name((action_t)-1), "?") == 0);
	report("action_name rejects N_ACTIONS", strcmp(action_name(N_ACTIONS), "?") == 0);
}

int main(void)
{
	puts("=== Transition table and lock_step: 16 cases ===");
	test_transitions();

	puts("\n=== lock_event: digits, close, timeout, invalid input ===");
	test_events();

	puts("\n=== Complete input sequences ===");
	test_sequences();

	puts("\n=== State, event, and action names ===");
	test_names();

	printf("\nSummary: %d passed, %d failed\n", passed, failed);
	return failed == 0 ? 0 : 1;
}
