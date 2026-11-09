#include "scenario.h"
#include "sensor.h"
#include "panel.h"
#include "iom361_r4.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

int main(int argc, char *argv[])
{
	// Step 1: Validate command-line arguments.
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <scenario-file>\n", argv[0]);
		return 1;
	}

	// Step 2: Load the scenario file.
	scenario_t scenario = {0};

	int rc = scenario_load(argv[1], &scenario);

	if (rc != 0)
	{
		fprintf(stderr, "Failed to load scenario\n");
		return 1;
	}

	if (scenario.count == 0)
	{
		fprintf(stderr, "Scenario contains no data entries\n");
		return 1;
	}

	// Step 3: Initialize four switches and four LEDs.
	rc = 0;
	uint32_t *base = iom361_initialize(4, 4, &rc);

	if (rc != 0 || base == NULL)
	{
		fprintf(stderr, "Failed to initialize simulated hardware\n");
		return 1;
	}

	// Disable automatic simulator display output.
	iom361_set_display(false);

	// Start with all four LEDs off.
	iom361_writeReg(base, LEDS_REG, 0u, &rc);

	if (rc != 0)
	{
		fprintf(stderr, "Failed to initialize LEDs\n");
		return 1;
	}

	// Step 4: Prepare the replay loop.
	int last_tick = scenario.lines[scenario.count - 1].tick;

	// Remember the switch state from the previous tick.
	uint32_t before = 0u;

	// Process every tick, including the final tick.
	for (int tick = 0; tick <= last_tick; tick++)
	{
		// Find the scenario values active at this tick.
		const scenario_line_t *entry =
			scenario_at_tick(&scenario, tick);

		if (entry == NULL)
		{
			fprintf(stderr,
				"No active scenario entry at tick %d\n",
				tick);
			return 1;
		}

		// Set the simulated world.
		// Confirm these two signatures in iom361_r4.h.
		_iom361_set_sensor1(entry->temp_c, entry->rh_pct);
		_iom361_set_switches(entry->switches);

		// Each tick represents one second.
		time_t now = scenario.start + (time_t)tick;

		// Read the sensor through hardware registers.
		reading_t reading = {0};

		rc = sensor_read(base, now, &reading);

		if (rc != 0)
		{
			fprintf(stderr,
				"Sensor read failed at tick %d (rc=%d)\n",
				tick, rc);
			return 1;
		}

		// Read switches through the register interface.
		rc = 0;
		uint32_t switches =
			iom361_readReg(base, SWITCHES_REG, &rc);

		if (rc != 0)
		{
			fprintf(stderr,
				"Switch read failed at tick %d\n",
				tick);
			return 1;
		}

		// Keep only the four installed switches.
		switches &= 0xFu;

		// Detect switches that changed from OFF to ON.
		uint32_t pressed = panel_pressed(switches, before);

		// Toggle one LED for every newly pressed switch.
		for (int k = 0; k < 4; k++)
		{
			if (pressed & (1u << k))
			{
				rc = panel_toggle(base, k);

				if (rc != 0)
				{
					fprintf(stderr,
						"LED toggle failed at tick %d, LED %d\n",
						tick, k);
					return 1;
				}
			}
		}

		// Read the actual LED register after toggling.
		rc = 0;
		uint32_t leds =
			iom361_readReg(base, LEDS_REG, &rc);

		if (rc != 0)
		{
			fprintf(stderr,
				"LED read failed at tick %d\n",
				tick);
			return 1;
		}

		// Convert the timestamp to UTC.
		struct tm *utc = gmtime(&reading.t);

		if (utc == NULL)
		{
			fprintf(stderr,
				"UTC conversion failed at tick %d\n",
				tick);
			return 1;
		}

		char when[32];

		if (strftime(when, sizeof(when),
			"%Y-%m-%d %H:%M:%S", utc) == 0)
		{
			fprintf(stderr,
				"Time formatting failed at tick %d\n",
				tick);
			return 1;
		}

		// Step 5: Print one trace line per tick.
		printf(
			"%s temp=%.1f rh=%.1f switches=0x%x pressed=0x%x leds=0x%x\n",
			when,
			(double)reading.temp_c,
			(double)reading.rh_pct,
			(unsigned int)switches,
			(unsigned int)pressed,
			(unsigned int)leds
		);

		// Remember the current switches for the next tick.
		before = switches;
	}

	return 0;
}
