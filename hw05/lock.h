/*	
* Header file for the lock state machine.
* Defines the states, events, and actions for the lock state machine, 
* as well as the transition table and function prototypes.
* Antonio Novelo - ECE-361 - 11/1/2026
*/

#ifndef LOCK_H
#define LOCK_H

typedef enum
{
	LOCKED,
	GOT1,
	GOT2,
	OPEN,
	N_STATES
} state_t;

typedef enum
{
	E_RIGHT,
	E_WRONG,
	E_CLOSE,
	E_TIMEOUT,
	N_EVENTS
} event_t;

typedef enum
{
	A_UNDEFINED,
	A_NONE,
	A_UNLOCK,
	A_LOCK,
	A_BEEP,
	N_ACTIONS
} action_t;

typedef struct
{
	state_t next;
	action_t action;
} transition_t;

extern const transition_t lock_table[N_STATES][N_EVENTS];


/* Look up the transition and return the next state. */
state_t lock_step(state_t s, event_t e, action_t *action);

/* Convert an input character into an event for the current state. */
int lock_event(state_t s, char c);

/* Return the name of a state as a string. */
//const char *state_name(state_t s);

/* Return the name of an event as a string. */
//const char *event_name(event_t e);

/* Return the name of an action as a string. */
//const char *action_name(action_t a);

#endif