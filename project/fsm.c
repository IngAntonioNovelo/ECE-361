
#include "fsm.h"

/* Each row represents a state.
 * Each column represents an event.
 * Each entry contains the next state and action.
 * This table dispatches events; event classification must enforce temperature
 * thresholds and the lockout deadline before requesting cooling.
 */
static const fsm_transition_t
transition_table[FSM_STATE_COUNT][EV_COUNT] = {
	/* Idle operation may start heating, cooling, or wait for lockout. */
	[FSM_OFF] = {
		[EV_SYSTEM_OFF] = {FSM_OFF, ACT_ALL_OFF},
		[EV_NEED_HEAT] = {FSM_HEATING, ACT_HEAT_ON},
		[EV_HEAT_DONE] = {FSM_OFF, ACT_KEEP},
		[EV_NEED_COOL] = {FSM_COOLING, ACT_COOL_ON},
		[EV_COOL_DONE] = {FSM_OFF, ACT_KEEP},
		[EV_COOL_BLOCKED] = {FSM_LOCKOUT, ACT_ALL_OFF},
		[EV_LOCKOUT_EXPIRED] = {FSM_OFF, ACT_KEEP},
		[EV_HOLD] = {FSM_OFF, ACT_KEEP}
	},

	/* Keep heating until a stop or another operating request is dispatched. */
	[FSM_HEATING] = {
		[EV_SYSTEM_OFF] = {FSM_OFF, ACT_ALL_OFF},
		[EV_NEED_HEAT] = {FSM_HEATING, ACT_KEEP},
		[EV_HEAT_DONE] = {FSM_OFF, ACT_ALL_OFF},
		[EV_NEED_COOL] = {FSM_COOLING, ACT_COOL_ON},
		[EV_COOL_DONE] = {FSM_HEATING, ACT_KEEP},
		[EV_COOL_BLOCKED] = {FSM_LOCKOUT, ACT_ALL_OFF},
		[EV_LOCKOUT_EXPIRED] = {FSM_HEATING, ACT_KEEP},
		[EV_HOLD] = {FSM_HEATING, ACT_KEEP}
	},

	/* Stopping the compressor requests a restart delay, including system-off. */
	[FSM_COOLING] = {
		[EV_SYSTEM_OFF] = {FSM_OFF, ACT_STOP_COOL_START_LOCKOUT},
		[EV_NEED_HEAT] = {FSM_COOLING, ACT_KEEP},
		[EV_HEAT_DONE] = {FSM_COOLING, ACT_KEEP},
		[EV_NEED_COOL] = {FSM_COOLING, ACT_KEEP},
		[EV_COOL_DONE] = {FSM_LOCKOUT, ACT_STOP_COOL_START_LOCKOUT},
		[EV_COOL_BLOCKED] = {FSM_COOLING, ACT_KEEP},
		[EV_LOCKOUT_EXPIRED] = {FSM_COOLING, ACT_KEEP},
		[EV_HOLD] = {FSM_COOLING, ACT_KEEP}
	},

	/* Cooling requests must be classified as blocked while the delay is active. */
	[FSM_LOCKOUT] = {
		[EV_SYSTEM_OFF] = {FSM_OFF, ACT_ALL_OFF},
		[EV_NEED_HEAT] = {FSM_HEATING, ACT_HEAT_ON},
		[EV_HEAT_DONE] = {FSM_LOCKOUT, ACT_KEEP},
		[EV_NEED_COOL] = {FSM_COOLING, ACT_COOL_ON},
		[EV_COOL_DONE] = {FSM_LOCKOUT, ACT_KEEP},
		[EV_COOL_BLOCKED] = {FSM_LOCKOUT, ACT_KEEP},
		[EV_LOCKOUT_EXPIRED] = {FSM_OFF, ACT_ALL_OFF},
		[EV_HOLD] = {FSM_LOCKOUT, ACT_KEEP}
	}
};

/* Retrieve a transition from the table. */
fsm_transition_t fsm_get_transition(
	fsm_state_t current_state,
	fsm_event_t event)
{
	/* Unsigned comparisons also reject negative enum values before indexing. */
	if ((unsigned)current_state >= (unsigned)FSM_STATE_COUNT ||
		(unsigned)event >= (unsigned)EV_COUNT)
	{
		/* Return an all-off request when either table index is invalid. */
		fsm_transition_t safe = {FSM_OFF, ACT_ALL_OFF};
		return safe;
	}

	return transition_table[current_state][event];
}

/* Return only the next state. */
fsm_state_t fsm_next_state(
	fsm_state_t current_state,
	fsm_event_t event)
{
	/* Reuse the checked lookup so both public functions handle bad inputs alike. */
	fsm_transition_t transition =
		fsm_get_transition(current_state, event);

	return transition.next_state;
}
