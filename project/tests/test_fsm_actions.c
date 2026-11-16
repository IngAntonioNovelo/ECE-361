
#include <assert.h>
#include <stdio.h>

#include "fsm.h"

/* Run event classification, table lookup, and action application as one tick.
   These checks use controller data without initializing the hardware model. */
static void run_tick(
	fsm_controller_t *controller,
	float temp,
	int system_on,
	time_t now)
{
	fsm_event_t event = fsm_determine_event(
		controller->state,
		temp,
		system_on,
		now,
		controller->lockout_until
	);

	/* Use the classified event to obtain both the destination and output action. */
	fsm_transition_t transition = fsm_get_transition(
		controller->state,
		event
	);

	/* Apply the action using the same timestamp used to classify the inputs. */
	assert(fsm_apply_transition(
		controller, transition, now) == 0);

	/* Furnace and compressor must never both be on. */
	assert(controller->leds != 0x3u);
}

/* Replay shutdown, restart, and heating during two compressor lockout periods. */
int main(void)
{
	fsm_controller_t controller;

	/* Initialization must begin idle with both actuator outputs disabled. */
	assert(fsm_controller_init(&controller) == 0);
	assert(controller.state == FSM_OFF);
	assert(controller.leds == 0u);

	/* Tick 79: high temperature starts cooling. */
	run_tick(&controller, 24.0f, 1, 79);
	assert(controller.state == FSM_COOLING);
	assert(controller.leds == FSM_LED_COMPRESSOR);

	/* Tick 80: cooling ends; lockout begins. */
	run_tick(&controller, 21.5f, 1, 80);
	assert(controller.state == FSM_LOCKOUT);
	assert(controller.lockout_until == 200);
	assert(controller.leds == 0u);

	/* Tick 81: system off must preserve lockout. */
	run_tick(&controller, 25.0f, 0, 81);
	assert(controller.state == FSM_OFF);
	assert(controller.lockout_until == 200);

	/* Tick 82: turning system on cannot bypass lockout. */
	run_tick(&controller, 25.0f, 1, 82);
	assert(controller.state == FSM_LOCKOUT);
	assert(controller.leds == 0u);

	/* Tick 199: compressor is still blocked. */
	run_tick(&controller, 25.0f, 1, 199);
	assert(controller.state == FSM_LOCKOUT);
	assert(controller.leds == 0u);

	/* Tick 200: cooling can restart. */
	run_tick(&controller, 25.0f, 1, 200);
	assert(controller.state == FSM_COOLING);
	assert(controller.leds == FSM_LED_COMPRESSOR);

	/* Tick 201: stopping again starts a fresh lockout. */
	run_tick(&controller, 21.5f, 1, 201);
	assert(controller.state == FSM_LOCKOUT);
	assert(controller.lockout_until == 321);

	/* Tick 202: heating is permitted during lockout. */
	run_tick(&controller, 19.0f, 1, 202);
	assert(controller.state == FSM_HEATING);
	assert(controller.leds == FSM_LED_FURNACE);
	assert(controller.lockout_until == 321);

	/* Tick 203: heating ends; lockout remains recorded. */
	run_tick(&controller, 21.5f, 1, 203);
	assert(controller.state == FSM_OFF);
	assert(controller.leds == 0u);
	assert(controller.lockout_until == 321);

	/* Tick 204: compressor still cannot run. */
	run_tick(&controller, 25.0f, 1, 204);
	assert(controller.state == FSM_LOCKOUT);
	assert(controller.leds == 0u);

	/* Tick 321: compressor may restart again. */
	run_tick(&controller, 25.0f, 1, 321);
	assert(controller.state == FSM_COOLING);
	assert(controller.leds == FSM_LED_COMPRESSOR);

	puts("PASS: FSM actions and lockout integration");

	return 0;
}
