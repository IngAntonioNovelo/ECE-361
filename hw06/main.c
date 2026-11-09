
#include "scenario.h"
#include "sensor.h"
#include "panel.h"
#include "iom361_r4.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
	// Step 1: Validate command-line arguments.
    if(argc != 2)
    {
        fprintf(stderr,
					"Usage: %s <scenario-file>\n", argv[0]);
		return 1;
    }

	// Step 2: Load the scenario.
	scenario_t scenario = {0};

	int rc = scenario_load(argv[1], &scenario);

	if (rc != 0)
	{
		fprintf(stderr, "Failed to load scenario\n");
		return 1;
	}

	// Step 3: Initialize the simulated hardware.
	uint32_t *base = iom361_initialize(4, 4, &rc);

	if (rc != 0 || base == NULL)
	{
	fprintf(stderr, "Failed to initialize simulated hardware\n");
	return 1;

	}

	iom361_set_display(false);

	// Step 4: Replay each tick.
	int last_tick = scenario.lines[scenario.count -1].tick;

	int tick = 0;

	for (int tick = 0; tick <= last_tick; tick++)
	{
		const scenario_line_t *entry = scenario_at_tick(&scenario, tick);
		
		if (entry == NULL)
		{
			fprintf(stderr, "No active scenario entry at tick %d\n", tick);
			return 1;
		}
		
	}

	// Step 5: Print results and clean up.
}
