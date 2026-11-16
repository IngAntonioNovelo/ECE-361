
#include <assert.h>
#include <stdio.h>

#include "fsm.h"

int main(void)
{
	fsm_transition_t t;

	t = fsm_get_transition(FSM_OFF, EV_NEED_HEAT);
	assert(t.next_state == FSM_HEATING);
	assert(t.action == ACT_HEAT_ON);

	t = fsm_get_transition(FSM_HEATING, EV_HEAT_DONE);
	assert(t.next_state == FSM_OFF);
	assert(t.action == ACT_ALL_OFF);

	t = fsm_get_transition(FSM_COOLING, EV_COOL_DONE);
	assert(t.next_state == FSM_LOCKOUT);
	assert(t.action == ACT_STOP_COOL_START_LOCKOUT);

	t = fsm_get_transition(FSM_LOCKOUT, EV_NEED_HEAT);
	assert(t.next_state == FSM_HEATING);
	assert(t.action == ACT_HEAT_ON);

	assert(fsm_next_state(FSM_OFF, EV_NEED_COOL)
		== FSM_COOLING);

	puts("FSM table checks passed");
	return 0;
}
