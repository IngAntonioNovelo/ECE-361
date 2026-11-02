/*	
* Source file for the lock state machine.
* Implements the states, events, and actions for the lock state machine, 
* as well as the transition table and function prototypes.
* Antonio Novelo - ECE-361 - 11/1/2026
*/

#include "lock.h"

const transition_t lock_table[N_STATES][N_EVENTS] = 
{
    // Define the transition table here
    // Current state, event -> { next state, action }
    [LOCKED] = 
    {
        [E_RIGHT] = { GOT1, A_NONE },
        [E_WRONG] = { LOCKED, A_BEEP },
        [E_CLOSE] = { LOCKED, A_NONE },
        [E_TIMEOUT] = { LOCKED, A_NONE }
    },
    
    [GOT1] = 
    {
	    [E_RIGHT]   = { GOT2, A_NONE },
	    [E_WRONG]   = { LOCKED, A_BEEP },
	    [E_CLOSE]   = { GOT1, A_NONE },
	    [E_TIMEOUT] = { LOCKED, A_NONE }
    },
    
    [GOT2] = 
    {
        [E_RIGHT]   = { OPEN, A_UNLOCK },
        [E_WRONG]   = { LOCKED, A_BEEP },
        [E_CLOSE]   = { GOT2, A_NONE },
        [E_TIMEOUT] = { LOCKED, A_NONE }
    },

    [OPEN] = 
    {
        [E_RIGHT]   = { OPEN, A_NONE },
        [E_WRONG]   = { OPEN, A_NONE },
        [E_CLOSE]   = { LOCKED, A_LOCK },
        [E_TIMEOUT] = { LOCKED, A_LOCK }
    },

};
    // Look up the transition and return the next state.
state_t lock_step(state_t s, event_t e, action_t *action) 
{
    transition_t t = lock_table[s][e];
    *action = t.action;
    return t.next;
}

// Return the next state given the current state and event.
int lock_event(state_t s, char c)
{
    // Check for the close event
    if (c == 'c')
    {
        return E_CLOSE;
    }
     
    // Check for the timeout event
    if (c == 't')
    {
        return E_TIMEOUT;
    }
       
    if (c >= '0' && c <= '9')
    {
        // Check for the correct first digit
        if (s == LOCKED && c == '3')
        {
            return E_RIGHT;
        }

        // Check for the correct second digit
        else if (s == GOT1 && c == '1')
        {
            return E_RIGHT;
        }

        // Check for the correct third digit
        else if (s == GOT2 && c == '4')
        {
            return E_RIGHT;
        }
        
        return E_WRONG;
    }
    
    return -1; // Invalid event
};

/* Return the name of a state as a string. */
//const char *state_name(state_t s);
// {    

//}


/* Return the name of an event as a string. */
//const char *event_name(event_t e);
// {

//}

/* Return the name of an action as a string. */
//const char *action_name(action_t a);
// {

//}