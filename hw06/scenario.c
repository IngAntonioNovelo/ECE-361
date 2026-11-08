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

	while (fgets(line, sizeof(line), fp) != NULL) // Read each line of the file
	{
		line_number++;

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

        // Next: Parse the line into two integers
        int now, before;
        if (sscanf(line, "%d %d", &now, &before) != 2) // If the line does not contain two integers, print an error and skip it
        {
            fprintf(stderr, "Error parsing line %d: '%s'\n", line_number, line); // Print an error message with the line number and the line content
            continue;
        }

        // Store the parsed integers in the scenario struct
        scenario->now[scenario->count] = now;
        scenario->before[scenario->count] = before;
        scenario->count++; // Increment the count of scenarios
        
	}

	fclose(fp); // Close the file
	return 0;
}
