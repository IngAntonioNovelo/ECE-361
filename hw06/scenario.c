
#include "scenario.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>


// Load a scenario from a file.
// Returns 0 on success, a physical line number for an invalid line,
// or -1 for a file-level error.
int scenario_load(const char *filename, scenario_t *scenario) // Load a scenario from a file.
{
	// Validate input pointers.
	if (filename == NULL || scenario == NULL) // Check for NULL pointers
	{
		fprintf(stderr, "Error: invalid scenario arguments\n");
		return -1;
	}

	// Open the file for reading.
	FILE *fp = fopen(filename, "r");

	if (fp == NULL) // Check if the file was opened successfully
	{
		perror("Failed to open scenario file");
		return -1;
	}

	char line[256];
	int line_number = 0;
	int have_start = 0;

	// Initially, no scenario entries have been loaded.
	scenario->count = 0;

	// Read one physical line at a time.
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		line_number++;

		// STEP 1: Check whether the line exceeded the buffer.
		size_t len = strlen(line);

		if (strchr(line, '\n') == NULL && 
			len == sizeof(line) - 1) // If the line is too long, it won't have a newline character and will fill the buffer
		{
			int next = fgetc(fp);

			// If more characters follow, the line is too long.
			if (next != '\n' && next != EOF)
			{
				fprintf(stderr,
					"Error: line %d is too long\n",
					line_number);

				fclose(fp);
				return line_number;
			}
		}

		// STEP 2: Remove comments beginning with '#'.
		char *hash = strchr(line, '#');

		if (hash != NULL) // If a '#' character is found, it indicates the start of a comment
		{
			*hash = '\0';
		}

		// STEP 3: Skip leading whitespace.
		char *p = line;

		while (*p != '\0' && isspace((unsigned char)*p)) // Skip whitespace characters at the beginning of the line
		{
			p++;
		}

		// Skip empty lines and comment-only lines.
		if (*p == '\0')
		{
			continue;
		}

		// STEP 4: Parse the required start timestamp.
		if (!have_start)
		{
			char keyword[16];
			long long start_seconds;
			char extra[2];

			int fields = sscanf(
				p,
				"%15s %lld %1s",
				keyword,
				&start_seconds,
				extra
			);

			// Valid format: start <timestamp>
			if (fields != 2 || strcmp(keyword, "start") != 0) // Check if the line has exactly two fields and the first field is "start"
			{
				fprintf(stderr,
					"Error parsing start timestamp at line %d: '%s'\n",
					line_number, line);

				fclose(fp);
				return line_number;
			}

			// Convert the timestamp into time_t.
			time_t converted = (time_t)start_seconds;

			// Verify that the conversion preserved the value.
			// This check is appropriate for the 64-bit WSL target.
			if ((long long)converted != start_seconds)
			{
				fprintf(stderr,
					"Error: timestamp out of range at line %d\n",
					line_number);

				fclose(fp);
				return line_number;
			}

			scenario->start = converted; // Store the start timestamp in the scenario structure
			have_start = 1;

			// The start line is not a scenario data entry.
			continue;
		}

		// STEP 5: Parse a scenario data entry.
		int tick;
		float temp_c;
		float rh_pct;
		unsigned switches;
		char extra[2];

		int fields = sscanf(
			p,
			"%d %f %f %x %1s",
			&tick,
			&temp_c,
			&rh_pct,
			&switches,
			extra
		);

		// Exactly four fields are required.
		if (fields != 4)
		{
			fprintf(stderr,
				"Error parsing line %d: '%s'\n",
				line_number, line);

			fclose(fp);
			return line_number;
		}

		// STEP 6: Reject negative ticks.
		if (tick < 0)
		{
			fprintf(stderr,
				"Error parsing line %d: negative tick %d\n",
				line_number, tick);

			fclose(fp);
			return line_number;
		}

		// STEP 7: Require strictly increasing ticks.
		if (scenario->count > 0 &&
			tick <= scenario->lines[scenario->count - 1].tick) // Ensure that the current tick is greater than the previous tick
		{
			fprintf(stderr,
				"Error parsing line %d: tick %d is not greater than previous tick %d\n",
				line_number,
				tick,
				scenario->lines[scenario->count - 1].tick); // Print an error message indicating that the current tick is not greater than the previous tick

			fclose(fp);
			return line_number;
		}

		// STEP 8: Protect the fixed-size array.
		if (scenario->count >= MAX_SCENARIO_LINES) // Check if the number of scenario entries exceeds the maximum allowed
		{
			fprintf(stderr,
				"Error parsing line %d: too many scenario entries (max %d)\n",
				line_number,
				MAX_SCENARIO_LINES); // Print an error message indicating that the maximum number of scenario entries has been exceeded

			fclose(fp);
			return line_number;
		}

		// STEP 9: Store the new scenario entry.
		scenario->lines[scenario->count].tick = tick; // Store the tick value in the scenario structure
		scenario->lines[scenario->count].temp_c = temp_c; // Store the temperature value in the scenario structure
		scenario->lines[scenario->count].rh_pct = rh_pct; // Store the relative humidity value in the scenario structure
		scenario->lines[scenario->count].switches = (uint32_t)switches; // Store the switches value in the scenario structure

		// Increment only after all fields have been stored.
		scenario->count++;
	}

	// STEP 10: Check for file-reading errors.
	if (ferror(fp))
	{
		fprintf(stderr, "Error reading scenario file\n");
		fclose(fp);
		return -1;
	}

	// STEP 11: Verify that a start timestamp was provided.
	if (!have_start)
	{
		fprintf(stderr,
			"Error: no start timestamp specified in scenario file\n");

		fclose(fp);
		return -1;
	}

	// Successfully finished reading the scenario.
	fclose(fp);
	return 0;
}


// Find the scenario entry active at the requested tick.
// Returns NULL if no entry is applicable.
const scenario_line_t *scenario_at_tick(const scenario_t *scenario, int tick) // Find the scenario entry active at the requested tick.
{
	// Validate the scenario and requested tick.
	if (scenario == NULL || scenario->count == 0 || tick < 0)
	{
		return NULL;
	}

	// Remember the most recent applicable entry.
	const scenario_line_t *latest = NULL;

	// Search the stored entries in chronological order.
	for (size_t i = 0; i < scenario->count; i++)
	{
		// Exact match: return the entry immediately.
		if (scenario->lines[i].tick == tick)
		{
			return &scenario->lines[i];
		}

		// Earlier entry: remember it as the latest valid entry.
		if (scenario->lines[i].tick < tick)
		{
			latest = &scenario->lines[i];
		}
		else
		{
			// Ticks are sorted, so no later entry can qualify.
			break;
		}
	}

	// Hold the last applicable value, or return NULL if none exists.
	return latest;
}
