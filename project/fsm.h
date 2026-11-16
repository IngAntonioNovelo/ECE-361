
#ifndef FSM_H
#define FSM_H

#include <time.h>

/* The four states required by Project 5. */
typedef enum {
	FSM_OFF,
	FSM_HEATING,
	FSM_COOLING,
	FSM_LOCKOUT,
	FSM_STATE_COUNT /* Table row count; not an operating state. */
} fsm_state_t;

/* Events that may trigger state transitions. */
typedef enum {
	EV_SYSTEM_OFF,
	EV_NEED_HEAT,
	EV_HEAT_DONE,
	EV_NEED_COOL,
	EV_COOL_DONE,
	EV_COOL_BLOCKED,
	EV_LOCKOUT_EXPIRED,
	EV_HOLD,
	EV_COUNT /* Table column count; not an event to dispatch. */
} fsm_event_t;

/* Action requests returned to the caller; table lookup does not drive hardware. */
typedef enum {
	ACT_KEEP, /* Leave the current outputs unchanged. */
	ACT_ALL_OFF,
	ACT_HEAT_ON,
	ACT_COOL_ON,
	ACT_STOP_COOL_START_LOCKOUT /* Stop cooling and begin the restart delay. */
} fsm_action_t;

/* One entry of our FSM transition table. */
typedef struct {
	fsm_state_t next_state;
	fsm_action_t action;
} fsm_transition_t;

/* Classify temperature, system enable, and lockout timing into an event.
   temp_c is in degrees Celsius; now and lockout_until use the same clock.
   This function is declared here but is not yet defined in fsm.c. */
fsm_event_t fsm_determine_event(
	fsm_state_t current_state,
	float temp_c,
	int system_on,
	time_t now,
	time_t lockout_until
);

/* Look up the next state and requested action without modifying hardware.
   Invalid state or event values return FSM_OFF with ACT_ALL_OFF. */
fsm_transition_t fsm_get_transition(
	fsm_state_t current_state,
	fsm_event_t event
);

/* Return only the next state, using the same validation as fsm_get_transition.
   Callers that need to apply outputs should retrieve the action as well. */
fsm_state_t fsm_next_state(
	fsm_state_t current_state,
	fsm_event_t event
);

#endif
