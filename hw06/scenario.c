#include "scenario.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>


int scenario_load(const char *filename, scenario_t *scenario) // Load a scenario from a file
{
	FILE *fp = fopen(filename, "r"); // Open the file for reading

	if (fp == NULL) // Check if the file was opened successfully
	{
		perror("Failed to open file");
		return -1;
	}

	char line[256]; // Buffer to hold each line of the file
	int line_number = 0;
    int have_start = 0; // Flag to indicate if the start time has been set

    size_t len = strlen(line);

    if (strchr(line, '\n') == NULL && len == sizeof(line) - 1) // Check if the line is too long
    {
        int next = fgetc(fp);
        if (next != '\n' && next != EOF) // If the next character is not a newline or EOF, the line is too long
        {
            fprintf(stderr, "Error: line %d is too long\n", line_number);
            fclose(fp);
            return line_number;
        }
    }

    scenario->count = 0; // Initialize the count of scenario lines

	while (fgets(line, sizeof(line), fp) != NULL) // Read each line of the file
	{
		line_number++;

        if (strchr(line, '\n') == NULL && !feof(fp)) // Check if the line is too long
        {
            fprintf(stderr, "Error: line %d is too long\n", line_number);
            fclose(fp);
            return line_number;
        }

		// Find the first '#' character
		char *hash = strchr(line, '#'); // Find the first '#' character

		// Remove the comment if one exists
		if (hash != NULL)
		{
			*hash = '\0';
		}

		// Next: Check for blank lines
        char *p = line;

        while (*p != '\0' && isspace((unsigned char)*p)) // Skip leading whitespace
        {
	        p++; // Move the pointer to the next character
        }

        if (*p == '\0') // If the line is blank, skip it
        {
            continue;
        }
    
        if(!have_start) // If the start time has not been set, parse the start time from the line
        {
        
            char keyword[16]; // Buffer to hold the keyword
            long long start_seconds; // Variable to hold the start seconds
            char extra[2]; // Buffer to hold any extra characters

            int fields = sscanf(p, "%15s %lld %1s", keyword, &start_seconds, extra); // Parse the line into a keyword, a long long integer, and an extra character

            if (fields != 2 || strcmp(keyword, "start") != 0) // If the line does not contain the keyword "start" followed by a long long integer, report an error and stop parsing.
            {
                fprintf(stderr, "Error parsing line %d: '%s'\n", line_number, line); // Print an error message with the line number and the line content
                fclose(fp); // Close the file
                return line_number; // Return the line number of the error
            }
        
            scenario->start = (time_t)start_seconds; // Store the start time in the scenario structure
            have_start = 1; // Set the flag to indicate that the start time has been set

            continue; // Move to the next line
        
	    }

        int tick;
        float temp_c;
        float rh_pct;
        unsigned switches;
        char extra[2];

        int fields = sscanf(p, "%d %f %f %x %1s", &tick, &temp_c, &rh_pct, &switches, extra); // Parse the line into a tick, temperature, relative humidity, switches, and an extra character
        
        if (fields != 4) // If the line does not contain the expected number of fields, report an error and stop parsing.
        {
            fprintf(stderr, "Error parsing line %d: '%s'\n", line_number, line); // Print an error message with the line number and the line content
            fclose(fp); // Close the file
            return line_number; // Return the line number of the error
        }
        
        if (tick < 0)
        {
            fprintf(stderr, "Error parsing line %d: negative tick %d\n",
                line_number, tick);
            fclose(fp);
            return line_number;
        }

        if (scenario -> count > 0 && tick <= scenario->lines[scenario->count - 1].tick) // If the tick is not greater than the previous tick, report an error and stop parsing.
        {
            fprintf(stderr, "Error parsing line %d: tick %d is not greater than previous tick %d\n", line_number, tick, scenario->lines[scenario->count - 1].tick); // Print an error message with the line number and the line content
            fclose(fp); // Close the file
            return line_number; // Return the line number of the error
        }

        if (scenario->count >= MAX_SCENARIO_LINES) // If the number of scenario lines exceeds the maximum, report an error and stop parsing.
        {
            fprintf(stderr, "Error parsing line %d: too many scenario lines (max %d)\n", line_number, MAX_SCENARIO_LINES); // Print an error message with the line number and the line content
            fclose(fp); // Close the file
            return line_number; // Return the line number of the error
        }

        scenario->lines[scenario->count].tick = tick; // Store the tick in the scenario structure
        scenario->lines[scenario->count].temp_c = temp_c; // Store the temperature in the scenario structure
        scenario->lines[scenario->count].rh_pct = rh_pct; // Store the relative humidity in the scenario structure
        scenario->lines[scenario->count].switches = switches; // Store the switches in the scenario structure

        scenario->count++; // Increment the count of scenario lines

    }
    
    if (ferror(fp)) // Check for errors while reading the file
    {
        perror("Error reading file");
        fclose(fp);
        return -1;
    }

    if (!have_start) // If the start time was not set, report an error and stop parsing.
    {
        fprintf(stderr, "Error: no start time specified in scenario file\n"); // Print an error message
        fclose(fp); // Close the file
        return -1; // Return an error code
    }
    
    fclose(fp); // Close the file
	return 0;
}

const scenario_line_t *scenario_at_tick(const scenario_t *scenario, int tick) // Find the scenario line with the given tick
{
    if (scenario == NULL || scenario->count == 0 || tick < 0) // If the scenario is NULL, has no lines, or the tick is negative, return NULL
    {
        return NULL;
    }

    const scenario_line_t *latest = NULL; // Pointer to hold the latest scenario line with a tick less than or equal to the given tick

    for (size_t i = 0; i < scenario->count; i++) // Iterate through the scenario lines
    {
        if (scenario->lines[i].tick == tick) // If the tick matches, return a pointer to the scenario line
        {
            return &scenario->lines[i];
        }
        if (scenario->lines[i].tick < tick) // If the tick is less than the given tick, update the latest pointer
        {
            latest = &scenario->lines[i];
        }
    }

    return latest; // If no exact match was found, return the latest scenario line
}