/*
 * ECE 361 HW04 additional ADT tests.
 * Build/run from the hw04 root using run_hw04_tests.sh.
 * All C block indentation uses tabs.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tally.h"

static int passed = 0;
static int failed = 0;

static void check_int(const char *label, int got, int expected)
{
	if (got == expected)
	{
		printf("PASS: %s (got %d)\n", label, got);
		passed++;
	}
	else
	{
		printf("FAIL: %s (expected %d, got %d)\n", label, expected, got);
		failed++;
	}
}

static void check_ptr(const char *label, const void *got, int should_be_null)
{
	int is_null = (got == NULL);
	if (is_null == should_be_null)
	{
		printf("PASS: %s\n", label);
		passed++;
	}
	else
	{
		printf("FAIL: %s (expected %s pointer)\n", label,
			should_be_null ? "NULL" : "non-NULL");
		failed++;
	}
}

static void check_str(const char *label, const char *got, const char *expected)
{
	if (strcmp(got, expected) == 0)
	{
		printf("PASS: %s\n", label);
		passed++;
	}
	else
	{
		printf("FAIL: %s\n", label);
		printf("  expected: <%s>\n", expected);
		printf("  got:      <%s>\n", got);
		failed++;
	}
}

static int capture_print(const tally_t *t, char *buf, size_t cap)
{
	FILE *f = tmpfile();
	if (f == NULL)
	{
		perror("tmpfile");
		return 0;
	}

	tally_print(t, f);
	if (fflush(f) != 0 || fseek(f, 0, SEEK_SET) != 0)
	{
		perror("rewind temporary output");
		fclose(f);
		return 0;
	}

	size_t used = fread(buf, 1, cap - 1, f);
	buf[used] = '\0';
	if (ferror(f))
	{
		perror("read temporary output");
		fclose(f);
		return 0;
	}
	fclose(f);
	return 1;
}

static void test_pool(void)
{
	puts("\n== Pool capacity and reuse ==");
	tally_t *t[5];
	for (int i = 0; i < 5; i++)
	{
		t[i] = tally_create();
	}
	for (int i = 0; i < 4; i++)
	{
		char label[64];
		snprintf(label, sizeof(label), "pool allocation %d succeeds", i + 1);
		check_ptr(label, t[i], 0);
	}
	check_ptr("fifth simultaneous allocation fails", t[4], 1);

	if (t[1] != NULL)
	{
		check_int("second tally accepts name", tally_add(t[1], "OLD"), 1);
		tally_destroy(t[1]);
		tally_t *reused = tally_create();
		check_ptr("destroyed pool slot can be reused", reused, 0);
		if (reused != NULL)
		{
			check_int("reused tally starts empty", tally_size(reused), 0);
			check_int("old name does not survive reuse", tally_count(reused, "OLD"), 0);
			tally_destroy(reused);
		}
	}
	for (int i = 0; i < 4; i++)
	{
		if (i != 1 && t[i] != NULL)
		{
			tally_destroy(t[i]);
		}
	}
	tally_destroy(NULL);
	puts("PASS: tally_destroy(NULL) did not crash");
	passed++;
}

static void test_counts_validation(void)
{
	puts("\n== Counts, size, and invalid inputs ==");
	check_int("tally_size(NULL) is zero", tally_size(NULL), 0);
	check_int("tally_count(NULL, name) is zero", tally_count(NULL, "A"), 0);

	tally_t *t = tally_create();
	check_ptr("create tally for count tests", t, 0);
	if (t == NULL)
	{
		return;
	}
	check_int("new tally has size zero", tally_size(t), 0);
	check_int("missing name count is zero", tally_count(t, "MISSING"), 0);
	check_int("NULL name count is zero", tally_count(t, NULL), 0);
	check_int("first BRAKE", tally_add(t, "BRAKE"), 1);
	check_int("second BRAKE", tally_add(t, "BRAKE"), 2);
	check_int("third BRAKE", tally_add(t, "BRAKE"), 3);
	check_int("count BRAKE is three", tally_count(t, "BRAKE"), 3);
	check_int("three BRAKE occurrences are one distinct name", tally_size(t), 1);
	check_int("empty name is rejected", tally_add(t, ""), -1);
	check_int("NULL name is rejected", tally_add(t, NULL), -1);
	check_int("NULL tally is rejected", tally_add(NULL, "BRAKE"), -1);
	check_int("15-character name is accepted", tally_add(t, "ABCDEFGHIJKLMNO"), 1);
	check_int("16-character name is rejected", tally_add(t, "ABCDEFGHIJKLMNOP"), -1);
	check_int("invalid additions leave size unchanged", tally_size(t), 2);
	check_int("invalid additions leave BRAKE unchanged", tally_count(t, "BRAKE"), 3);
	tally_destroy(t);
}

static void test_sorted_print(void)
{
	puts("\n== Sorted printing and empty output ==");
	tally_t *t = tally_create();
	check_ptr("create tally for print tests", t, 0);
	if (t == NULL)
	{
		return;
	}
	char buf[2048];
	if (capture_print(t, buf, sizeof(buf)))
	{
		check_str("empty tally prints nothing", buf, "");
	}
	else
	{
		puts("FAIL: cannot capture empty output");
		failed++;
	}
	tally_add(t, "TEMP");
	tally_add(t, "BRAKE");
	tally_add(t, "ALARM");
	tally_add(t, "AXLE");
	tally_add(t, "BRAKE");
	if (capture_print(t, buf, sizeof(buf)))
	{
		check_str("printing is alphabetical with exact counts/newlines", buf,
			"ALARM 1\nAXLE 1\nBRAKE 2\nTEMP 1\n");
	}
	else
	{
		puts("FAIL: cannot capture populated output");
		failed++;
	}
	if (capture_print(NULL, buf, sizeof(buf)))
	{
		check_str("NULL tally prints nothing", buf, "");
	}
	else
	{
		puts("FAIL: cannot capture NULL tally output");
		failed++;
	}
	tally_print(t, NULL);
	puts("PASS: NULL output stream did not crash");
	passed++;
	tally_destroy(t);
}

static void test_full_tally(void)
{
	puts("\n== 16 distinct names and full capacity ==");
	tally_t *t = tally_create();
	check_ptr("create tally for capacity tests", t, 0);
	if (t == NULL)
	{
		return;
	}
	for (int i = 15; i >= 0; i--)
	{
		char name[16];
		snprintf(name, sizeof(name), "EV%02d", i);
		int result = tally_add(t, name);
		if (result != 1)
		{
			printf("FAIL: inserting name %s (got %d)\n", name, result);
			failed++;
		}
	}
	check_int("16 distinct names accepted", tally_size(t), 16);
	check_int("17th distinct name rejected", tally_add(t, "EV16"), -1);
	check_int("rejection does not change size", tally_size(t), 16);
	check_int("existing name increments even when full", tally_add(t, "EV00"), 2);
	check_int("count matches after full tally update", tally_count(t, "EV00"), 2);
	char buf[2048];
	char expected[2048] = "";
	for (int i = 0; i < 16; i++)
	{
		char row[32];
		snprintf(row, sizeof(row), "EV%02d %d\n", i, i == 0 ? 2 : 1);
		strcat(expected, row);
	}
	if (capture_print(t, buf, sizeof(buf)))
	{
		check_str("full tally prints all 16 in alphabetical order", buf, expected);
	}
	else
	{
		puts("FAIL: cannot capture full tally output");
		failed++;
	}
	tally_destroy(t);
}

int main(void)
{
	test_pool();
	test_counts_validation();
	test_sorted_print();
	test_full_tally();
	printf("\nADT SUMMARY: %d passed, %d failed\n", passed, failed);
	return failed == 0 ? 0 : 1;
}
