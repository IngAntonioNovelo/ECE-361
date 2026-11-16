
#ifndef FSM_H
#define FSM_H

#include <time.h>
#include <stdint.h>

/* The four states required by Project 5. */
typedef enum {
	FSM_OFF,     /* Neither furnace nor compressor is running. */
	FSM_HEATING, /* Furnace is running. */
	FSM_COOLING, /* Compressor is running. */
	FSM_LOCKOUT, /* Compressor restart is delayed; heating may still be requested. */
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
   Zero system_on requests shutoff; any nonzero value enables regulation.
   Cooling may restart when now is equal to or later than lockout_until. */
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


/* Compressor restart protection, in seconds. */
enum {
	FSM_LOCKOUT_TICKS = 120
};

/* LED register bit masks. */
#define FSM_LED_FURNACE    (1u << 0)
#define FSM_LED_COMPRESSOR (1u << 1)

/* Persistent controller information. */
typedef struct {
	fsm_state_t state;    /* Current operating mode. */
	time_t lockout_until; /* Earliest restart time, retained across system-off. */
	uint32_t leds;        /* Desired LED register value; no hardware write occurs here. */
} fsm_controller_t;

/* Initialize OFF with no outputs or pending lockout.
   Return 0 on success, or -1 for a NULL controller. */
int fsm_controller_init(fsm_controller_t *controller);

/* Apply a classified transition to the controller's stored outputs and state.
   Return 0 on success, or -1 for NULL, an invalid next state, or unknown action.
   The caller must obtain a transition appropriate to the inputs and deadline;
   this function does not independently enforce the compressor lockout. */
int fsm_apply_transition(
	fsm_controller_t *controller,
	fsm_transition_t transition,
	time_t now
);


/* Return only the next state, using the same validation as fsm_get_transition.
   Callers that need to apply outputs should retrieve the action as well. */
fsm_state_t fsm_next_state(
	fsm_state_t current_state,
	fsm_event_t event
);

#endif
