
#include "scenario.h"

#include <assert.h>
#include <stdio.h>

// Write a temporary scenario file for testing.
static void write_test_file(const char *filename, const char *contents)
{
	FILE *fp = fopen(filename, "w");
	assert(fp != NULL);

	assert(fputs(contents, fp) >= 0);
	assert(fclose(fp) == 0);
}

int main(void)
{
	const char *filename = "tests/scenario_test_temp.txt";
	scenario_t scenario = {0};

	// TEST 1: Valid file with comments and blank lines.
	write_test_file(filename,
		"# Example scenario\n"
		"\n"
		"start 1791273600\n"
		"0 20.0 35.0 0x0\n"
		"4 30.0 50.0 0x2\n"
		"8 25.0 45.0 0x0\n");

	assert(scenario_load(filename, &scenario) == 0);
	assert(scenario.count == 3);
	puts("PASS: valid scenario loading");

	// TEST 2: Exact matches and hold-last-value behavior.
	assert(scenario_at_tick(&scenario, 0) == &scenario.lines[0]);
	assert(scenario_at_tick(&scenario, 4) == &scenario.lines[1]);
	assert(scenario_at_tick(&scenario, 6) == &scenario.lines[1]);
	assert(scenario_at_tick(&scenario, 10) == &scenario.lines[2]);
	puts("PASS: scenario lookup");

	// TEST 3: Duplicate ticks must be rejected.
	write_test_file(filename,
		"start 1791273600\n"
		"0 20.0 35.0 0x0\n"
		"4 30.0 50.0 0x2\n"
		"4 25.0 45.0 0x0\n");

	assert(scenario_load(filename, &scenario) == 4);
	puts("PASS: duplicate tick rejected");

	// TEST 4: Lookup before the first entry returns NULL.
	write_test_file(filename,
		"start 1791273600\n"
		"4 22.0 40.0 0x0\n"
		"8 25.0 45.0 0x1\n");

	assert(scenario_load(filename, &scenario) == 0);
	assert(scenario_at_tick(&scenario, 2) == NULL);
	puts("PASS: lookup before first entry");

	// TEST 5: Missing start timestamp.
	write_test_file(filename,
		"# Comment only\n"
		"\n");

	assert(scenario_load(filename, &scenario) != 0);
	puts("PASS: missing start rejected");

    
	// TEST 6: Missing scenario field.
	write_test_file(filename,
		"start 1791273600\n"
		"0 20.0 35.0 0x0\n"
		"1 25.0 40.0\n");

	assert(scenario_load(filename, &scenario) == 3);
	puts("PASS: missing field rejected");

	// TEST 7: Decreasing ticks.
	write_test_file(filename,
		"start 1791273600\n"
		"0 20.0 35.0 0x0\n"
		"5 30.0 50.0 0x2\n"
		"3 25.0 45.0 0x0\n");

	assert(scenario_load(filename, &scenario) == 4);
	puts("PASS: decreasing tick rejected");

	// TEST 8: Negative tick.
	write_test_file(filename,
		"start 1791273600\n"
		"-1 20.0 35.0 0x0\n");

	assert(scenario_load(filename, &scenario) == 2);
	puts("PASS: negative tick rejected");

	// TEST 9: A line longer than the input buffer.
	FILE *fp = fopen(filename, "w");
	assert(fp != NULL);

	assert(fputs("start 1791273600\n", fp) >= 0);

	for (int i = 0; i < 300; i++)
	{
		assert(fputc('X', fp) != EOF);
	}

	assert(fputc('\n', fp) != EOF);
	assert(fclose(fp) == 0);

	assert(scenario_load(filename, &scenario) == 2);
	puts("PASS: overlong line rejected");


	// Remove the temporary test file.
	assert(remove(filename) == 0);

	puts("All scenario tests passed!");
	return 0;
}
