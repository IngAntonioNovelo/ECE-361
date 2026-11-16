
#include <assert.h>
#include <stdio.h>

#include "fsm.h"

/* Count event assertions separately from the two action assertions in main. */
static int test_count = 0;

/* Check that an input produces the expected event. */
static void expect_event(
	fsm_state_t state,
	float temp,
	int on,
	time_t now,
	time_t until,
	fsm_event_t expected)
{
	fsm_event_t actual =
		fsm_determine_event(state, temp, on, now, until);

	/* A failed assertion terminates the test; only successful cases are counted. */
	assert(actual == expected);
	test_count++;
}

/* Exercise temperature boundaries, timer expiry, overrides, and safe fallback. */
int main(void)
{
	/* Exact temperature thresholds. */
	expect_event(FSM_OFF, 19.9f, 1, 100, 0,
		EV_NEED_HEAT);
	expect_event(FSM_OFF, 20.0f, 1, 100, 0,
		EV_HOLD);

	expect_event(FSM_HEATING, 21.4f, 1, 100, 0,
		EV_HOLD);
	expect_event(FSM_HEATING, 21.5f, 1, 100, 0,
		EV_HEAT_DONE);

	expect_event(FSM_OFF, 23.0f, 1, 100, 0,
		EV_HOLD);
	expect_event(FSM_OFF, 23.1f, 1, 100, 0,
		EV_NEED_COOL);

	expect_event(FSM_COOLING, 21.6f, 1, 80, 0,
		EV_HOLD);
	expect_event(FSM_COOLING, 21.5f, 1, 80, 0,
		EV_COOL_DONE);

	/* Cooling stopped at tick 80. */
	time_t stop_time = 80;
	time_t unlock_time = stop_time + 120;

	/* Compare one second before expiry with the exact deadline (tick 200). */
	expect_event(FSM_OFF, 25.0f, 1, 199,
		unlock_time, EV_COOL_BLOCKED);
	expect_event(FSM_OFF, 25.0f, 1, 200,
		unlock_time, EV_NEED_COOL);

	/* An existing lockout holds its state until the same deadline is reached. */
	expect_event(FSM_LOCKOUT, 25.0f, 1, 199,
		unlock_time, EV_HOLD);
	expect_event(FSM_LOCKOUT, 25.0f, 1, 200,
		unlock_time, EV_NEED_COOL);

	/* Lockout expires without needing cooling. */
	expect_event(FSM_LOCKOUT, 21.0f, 1, 200,
		unlock_time, EV_LOCKOUT_EXPIRED);

	/* Heating is permitted during compressor lockout. */
	expect_event(FSM_LOCKOUT, 19.5f, 1, 199,
		unlock_time, EV_NEED_HEAT);

	/* A hot jump while heating requests cooling only after its timer permits it. */
	expect_event(FSM_HEATING, 24.0f, 1, 199,
		unlock_time, EV_COOL_BLOCKED);
	expect_event(FSM_HEATING, 24.0f, 1, 200,
		unlock_time, EV_NEED_COOL);

	/* Turning SYSTEM_ON off overrides temperature. */
	expect_event(FSM_COOLING, 24.0f, 0, 150, 0,
		EV_SYSTEM_OFF);
	expect_event(FSM_LOCKOUT, 25.0f, 0, 199,
		unlock_time, EV_SYSTEM_OFF);

	/* A sudden drop stops cooling first. */
	expect_event(FSM_COOLING, 19.0f, 1, 80, 0,
		EV_COOL_DONE);

	/* Invalid states fail safely. */
	expect_event((fsm_state_t)-1, 21.0f, 1, 100, 0,
		EV_SYSTEM_OFF);

	/* Check actions associated with stopping cooling. */
	assert(fsm_get_transition(FSM_COOLING,
		EV_COOL_DONE).action
		== ACT_STOP_COOL_START_LOCKOUT);

	assert(fsm_get_transition(FSM_COOLING,
		EV_SYSTEM_OFF).action
		== ACT_STOP_COOL_START_LOCKOUT);

	printf("PASS: %d event cases, 2 action checks\n",
		test_count);

	return 0;
}
