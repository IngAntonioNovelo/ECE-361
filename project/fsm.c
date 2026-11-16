
#include "fsm.h"
#include <stddef.h>

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


/* Initialize the thermostat controller. */
int fsm_controller_init(fsm_controller_t *controller)
{
	if (controller == NULL)
	{
		return -1;
	}

	controller->state = FSM_OFF;
	/* Zero represents no pending restart delay at initialization. */
	controller->lockout_until = 0;
	controller->leds = 0u;

	return 0;
}

/* Execute a transition and update controller state. */
int fsm_apply_transition(
	fsm_controller_t *controller,
	fsm_transition_t transition,
	time_t now)
{
	if (controller == NULL)
	{
		return -1;
	}

	/* Check the next state before using it. */
	if ((unsigned)transition.next_state >=
		(unsigned)FSM_STATE_COUNT)
	{
		return -1;
	}

	switch (transition.action)
	{
		case ACT_KEEP:
			/* Leave LED outputs unchanged. */
			break;

		case ACT_ALL_OFF:
			/* Preserve any recorded compressor deadline while disabling outputs. */
			controller->leds = 0u;
			break;

		case ACT_HEAT_ON:
			/* Assign the whole mask so heating also clears the compressor bit. */
			controller->leds = FSM_LED_FURNACE;
			break;

		case ACT_COOL_ON:
			/* Assign the whole mask so cooling also clears the furnace bit. */
			controller->leds = FSM_LED_COMPRESSOR;
			break;

		case ACT_STOP_COOL_START_LOCKOUT:
			/* Start a fresh 120-second delay measured from this tick. */
			controller->leds = 0u;
			controller->lockout_until =
				now + FSM_LOCKOUT_TICKS;
			break;

		default:
			/* Reject unknown actions. */
			return -1;
	}

	/* Save the new state after executing the action. */
	controller->state = transition.next_state;

	return 0;
}


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

/* Convert thermostat inputs into an FSM event without changing outputs or time.
   fsm_apply_transition records lockout_until for a compressor-stop action. */
fsm_event_t fsm_determine_event(
	fsm_state_t current_state,
	float temp_c,
	int system_on,
	time_t now,
	time_t lockout_until)
{
	/* System shutoff has the highest priority. */
	if (!system_on)
	{
		return EV_SYSTEM_OFF;
	}

	/* State-dependent stop thresholds provide hysteresis between start and stop. */
	switch (current_state)
	{
		case FSM_OFF:
			/* Start heat strictly below 20 C; exactly 20 C remains idle. */
			if (temp_c < 20.0f)
			{
				return EV_NEED_HEAT;
			}

			/* Start cooling strictly above 23 C, provided its delay has ended. */
			if (temp_c > 23.0f)
			{
				if (now < lockout_until)
				{
					return EV_COOL_BLOCKED;
				}

				return EV_NEED_COOL;
			}

			break;

		case FSM_HEATING:
			/* Hysteresis keeps heat running until the 21.5 C stop threshold. */
			if (temp_c >= 21.5f)
			{
				/* A jump above 23 C requests cooling immediately, subject to lockout. */
				if (temp_c > 23.0f)
				{
					if (now < lockout_until)
					{
						return EV_COOL_BLOCKED;
					}

					return EV_NEED_COOL;
				}

				return EV_HEAT_DONE;
			}

			break;

		case FSM_COOLING:
			/* Stop cooling at or below 21.5 C; the table requests a new lockout. */
			if (temp_c <= 21.5f)
			{
				return EV_COOL_DONE;
			}

			break;

		case FSM_LOCKOUT:
			/* The compressor delay does not prevent a request for heating. */
			if (temp_c < 20.0f)
			{
				return EV_NEED_HEAT;
			}

			/* The deadline itself permits cooling; before it, hold this state. */
			if (now >= lockout_until)
			{
				if (temp_c > 23.0f)
				{
					return EV_NEED_COOL;
				}

				/* With no cooling demand, expiry returns the controller to idle. */
				return EV_LOCKOUT_EXPIRED;
			}

			break;

		default:
			/* An invalid state should stop the system. */
			return EV_SYSTEM_OFF;
	}

	/* The current operating mode continues when no threshold or deadline applies. */
	return EV_HOLD;
}
