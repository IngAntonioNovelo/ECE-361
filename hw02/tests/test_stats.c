#include "stats.h"
#include <stdio.h>

static int passed = 0;
static int failed = 0;

static void check(const char *name, int ok)
{
    printf("%s  %s\n", ok ? "PASS" : "FAIL", name);
    if (ok)
        passed++;
    else
        failed++;
}

static void test_single_value(void)
{
    const float a[] = { 12.5f };
    int start = -99;
    check("one value: min/max/mean", stats_min(a, 1) == 12.5f &&
          stats_max(a, 1) == 12.5f && stats_mean(a, 1) == 12.5f);
    check("one value: longest run", stats_longest_run_above(a, 1, 10.0f, &start) == 1 && start == 0);
}

static void test_max_at_start_and_end(void)
{
    const float first[] = { 9.0f, 2.0f, 4.0f };
    const float last[] = { 2.0f, 4.0f, 9.0f };
    check("maximum at start", stats_max(first, 3) == 9.0f);
    check("maximum at end", stats_max(last, 3) == 9.0f);
}

static void test_exact_threshold(void)
{
    const float a[] = { 31.0f, 32.0f, 30.0f, 33.0f, 34.0f };
    int start = -99;
    check("equal threshold ends run", stats_longest_run_above(a, 5, 30.0f, &start) == 2 && start == 0);
}

static void test_no_run(void)
{
    const float a[] = { 25.0f, 30.0f };
    int start = -99;
    check("no run above threshold", stats_longest_run_above(a, 2, 30.0f, &start) == 0 && start == -1);
}

static void test_large_recursive_max(void)
{
    float a[10000];
    for (int i = 0; i < 10000; i++)
        a[i] = (float)i;
    check("recursive max: 10000 values", stats_max(a, 10000) == 9999.0f);
}

int main(void)
{
    test_single_value();
    test_max_at_start_and_end();
    test_exact_threshold();
    test_no_run();
    test_large_recursive_max();
    printf("Summary: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
