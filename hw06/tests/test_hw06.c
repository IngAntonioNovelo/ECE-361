#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "scenario.h"

#include "panel.h"
#include "iom361_r4.h"

static int tests_passed = 0;
static int tests_failed = 0;

static void check(const char *name, bool condition)
{
	if (condition)
	{
		printf("PASS  %s\n", name);
		tests_passed++;
	}
	else
	{
		printf("FAIL  %s\n", name);
		tests_failed++;
	}
}


static void test_scenario_lookup(void)
{
	scenario_t scenario = {0};

	scenario.count = 2;

	scenario.lines[0].tick = 4;
	scenario.lines[0].temp_c = 22.0f;

	scenario.lines[1].tick = 8;
	scenario.lines[1].temp_c = 25.0f;

	const scenario_line_t *r;

	r = scenario_at_tick(&scenario, 2);
	check("scenario: before first tick returns NULL",
		r == NULL);

	r = scenario_at_tick(&scenario, 4);
	check("scenario: exact match at tick 4",
		r == &scenario.lines[0]);

	r = scenario_at_tick(&scenario, 6);
	check("scenario: holds values between ticks",
		r == &scenario.lines[0]);

	r = scenario_at_tick(&scenario, 8);
	check("scenario: exact match at tick 8",
		r == &scenario.lines[1]);

	r = scenario_at_tick(&scenario, 10);
	check("scenario: holds values after final tick",
		r == &scenario.lines[1]);
}


int main(void)
{
	int rc = 0;
	uint32_t *base = iom361_initialize(4, 4, &rc);

	if (rc != 0 || base == NULL)
	{
		printf("FAIL  hardware initialization (rc=%d)\n", rc);
		return 1;
	}

	iom361_set_display(false);

	// Press detection: new press, held switch, and no press.
	check("pressed: new switch 1", panel_pressed(0x3u, 0x1u) == 0x2u);
	check("pressed: held switch", panel_pressed(0x3u, 0x3u) == 0x0u);
	check("pressed: release is not a press", panel_pressed(0x1u, 0x3u) == 0x0u);

	// Release detection: release, held switch, and new press.
	check("released: switch 2", panel_released(0x2u, 0x6u) == 0x4u);
	check("released: held switch", panel_released(0x3u, 0x3u) == 0x0u);
	check("released: press is not a release", panel_released(0x3u, 0x1u) == 0x0u);

	// Set LEDs 0 and 2 ON, then turn LED 1 ON without clearing them.
	iom361_writeReg(base, LEDS_REG, 0x5u, &rc);
	check("setup: initialize LED bits", rc == 0);
	if (rc != 0)
	{
		printf("Summary: %d passed, %d failed\n", tests_passed, tests_failed);
		return 1;
	}

	int action_rc = panel_led(base, 1, 1);
	int read_rc = 0;
	uint32_t actual = iom361_readReg(base, LEDS_REG, &read_rc);
	check("panel_led: ON preserves other LEDs",
		action_rc == 0 && read_rc == 0 && actual == 0x7u);

	// Turn LED 1 OFF; bits 0 and 2 should remain ON.
	action_rc = panel_led(base, 1, 0);
	read_rc = 0;
	actual = iom361_readReg(base, LEDS_REG, &read_rc);
	check("panel_led: OFF preserves other LEDs",
		action_rc == 0 && read_rc == 0 && actual == 0x5u);

	// Toggle LED 1 twice; it should return to its original state.
	action_rc = panel_toggle(base, 1);
	read_rc = 0;
	actual = iom361_readReg(base, LEDS_REG, &read_rc);
	check("panel_toggle: first toggle 0x5 to 0x7",
		action_rc == 0 && read_rc == 0 && actual == 0x7u);

	action_rc = panel_toggle(base, 1);
	read_rc = 0;
	actual = iom361_readReg(base, LEDS_REG, &read_rc);
	check("panel_toggle: second toggle 0x7 to 0x5",
		action_rc == 0 && read_rc == 0 && actual == 0x5u);

	// Invalid LED numbers must be rejected before shifting.
	check("panel_led: rejects negative LED", panel_led(base, -1, 1) == -1);
	check("panel_led: rejects LED 32", panel_led(base, 32, 1) == -1);
	check("panel_toggle: rejects negative LED", panel_toggle(base, -1) == -1);
	check("panel_toggle: rejects LED 32", panel_toggle(base, 32) == -1);

	test_scenario_lookup();

	printf("Summary: %d passed, %d failed\n", tests_passed, tests_failed);
	return tests_failed == 0 ? 0 : 1;
}