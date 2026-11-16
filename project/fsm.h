
#ifndef FSM_H
#define FSM_H

#include <time.h>

/* The four states required by Project 5. */
typedef enum {
	FSM_OFF,
	FSM_HEATING,
	FSM_COOLING,
	FSM_LOCKOUT,
	FSM_STATE_COUNT
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
	EV_COUNT
} fsm_event_t;

/* Actions performed when a transition occurs. */
typedef enum {
	ACT_KEEP,
	ACT_ALL_OFF,
	ACT_HEAT_ON,
	ACT_COOL_ON,
	ACT_STOP_COOL_START_LOCKOUT
} fsm_action_t;

/* One entry of our FSM transition table. */
typedef struct {
	fsm_state_t next_state;
	fsm_action_t action;
} fsm_transition_t;

/* Determine which event is occurring. */
fsm_event_t fsm_determine_event(
	fsm_state_t current_state,
	float temp_c,
	int system_on,
	time_t now,
	time_t lockout_until
);

/* Get both the next state and its action. */
fsm_transition_t fsm_get_transition(
	fsm_state_t current_state,
	fsm_event_t event
);

/* Return only the next state. */
fsm_state_t fsm_next_state(
	fsm_state_t current_state,
	fsm_event_t event
);

#endif
