
#include <stdio.h>
#include <stddef.h>
#include "ptr.h"

static int passed = 0;
static int failed = 0;

static void check(int condition, const char *name)
{
    if (condition)
    {
        printf("PASS: %s\n", name);
        passed++;
    }
    else
    {
        printf("FAIL: %s\n", name);
        failed++;
    }
}

int main(void)
{
    // TEST 1: swap_float
    float x = 10.0f;
    float y = 20.0f;

    swap_float(&x, &y);
    check(x == 20.0f && y == 10.0f,
          "swap_float");

    // TEST 2: minmax - multiple values
    float values[] = {8.0f, 3.0f, 12.0f, 5.0f};
    float smallest = 0.0f;
    float largest = 0.0f;

    int result = minmax(values, 4, &smallest, &largest);
    check(result == 0 && smallest == 3.0f &&
          largest == 12.0f, "minmax multiple values");

    // TEST 3: minmax - single element
    float single[] = {7.5f};

    result = minmax(single, 1, &smallest, &largest);
    check(result == 0 && smallest == 7.5f &&
          largest == 7.5f, "minmax single element");

    // TEST 4: minmax - n = 0
    smallest = 100.0f;
    largest = 200.0f;

    result = minmax(values, 0, &smallest, &largest);
    check(result == -1 && smallest == 100.0f &&
          largest == 200.0f, "minmax n = 0");

    // TEST 5: minmax - NULL array
    result = minmax(NULL, 4, &smallest, &largest);
    check(result == -1 && smallest == 100.0f &&
          largest == 200.0f, "minmax NULL array");

    // TEST 6: minmax - NULL minimum
    result = minmax(values, 4, NULL, &largest);
    check(result == -1 && largest == 200.0f,
          "minmax NULL minimum");

    // TEST 7: minmax - NULL maximum
    result = minmax(values, 4, &smallest, NULL);
    check(result == -1 && smallest == 100.0f,
          "minmax NULL maximum");

    // TEST 8: find_above - first match
    float search[] = {4.0f, 8.0f, 12.0f, 6.0f};

    const float *found = find_above(search, 4, 7.0f);
    check(found == search + 1, "find_above first match");

    // TEST 9: find_above - no match
    found = find_above(search, 4, 13.0f);
    check(found == NULL, "find_above no match");

    // TEST 10: find_above - equal threshold
    found = find_above(search, 4, 8.0f);
    check(found == search + 2,
          "find_above equal threshold");

    // TEST 11: reverse - even length
    float even[] = {1.0f, 2.0f, 3.0f, 4.0f};

    reverse(even, 4);
    check(even[0] == 4.0f && even[1] == 3.0f &&
          even[2] == 2.0f && even[3] == 1.0f,
          "reverse even length");

    // TEST 12: reverse - odd length
    float odd[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    reverse(odd, 5);
    check(odd[0] == 5.0f && odd[1] == 4.0f &&
          odd[2] == 3.0f && odd[3] == 2.0f &&
          odd[4] == 1.0f, "reverse odd length");

    // TEST 13: reverse - single element
    float one[] = {42.0f};

    reverse(one, 1);
    check(one[0] == 42.0f, "reverse single element");

    // TEST 14: reverse - NULL array
    reverse(NULL, 4);
    check(1, "reverse NULL did not crash");

    printf("\nSummary: %d passed, %d failed\n",
           passed, failed);

    return failed == 0 ? 0 : 1;
}
