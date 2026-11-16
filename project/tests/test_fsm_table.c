
#include <assert.h>
#include <stdio.h>

#include "fsm.h"

/* Verify representative table entries and the next-state convenience API. */
int main(void)
{
	fsm_transition_t t;

	/* Heating demand from idle must request the furnace as well as its state. */
	t = fsm_get_transition(FSM_OFF, EV_NEED_HEAT);
	assert(t.next_state == FSM_HEATING);
	assert(t.action == ACT_HEAT_ON);

	/* Reaching the heating stop threshold turns the outputs off. */
	t = fsm_get_transition(FSM_HEATING, EV_HEAT_DONE);
	assert(t.next_state == FSM_OFF);
	assert(t.action == ACT_ALL_OFF);

	/* Stopping cooling must also request the compressor restart timer. */
	t = fsm_get_transition(FSM_COOLING, EV_COOL_DONE);
	assert(t.next_state == FSM_LOCKOUT);
	assert(t.action == ACT_STOP_COOL_START_LOCKOUT);

	/* Compressor protection permits heating while cooling remains restricted. */
	t = fsm_get_transition(FSM_LOCKOUT, EV_NEED_HEAT);
	assert(t.next_state == FSM_HEATING);
	assert(t.action == ACT_HEAT_ON);

	/* The convenience function returns the same destination as table lookup. */
	assert(fsm_next_state(FSM_OFF, EV_NEED_COOL)
		== FSM_COOLING);

	puts("FSM table checks passed");
	return 0;
}
