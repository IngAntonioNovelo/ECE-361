#include <stdint.h>
#include <stdio.h>
#include "bits.h"

static int fails = 0;

#define CHECK(cond) do {                                      \
    if (cond)                                                 \
        printf("PASS: %s\n", #cond);                          \
    else {                                                    \
        printf("FAIL: %s (line %d)\n", #cond, __LINE__);      \
        fails++;                                              \
    }                                                         \
} while (0)

int main(void)
{
    printf("Testing print_binary:\n");
    print_binary(0x2Cu, 8);
    printf("   expected: 0010 1100\n\n");

    printf("Testing get_field:\n");
    CHECK(get_field(0xB6C5u, 4, 4) == 0xCu);
    CHECK(get_field(0x12345678u, 0, 32) == 0x12345678u);
    CHECK(get_field(0xFFFFFFFFu, 0, 1) == 1u);
    CHECK(get_field(0x12345678u, 31, 2) == 0u);

    printf("\nTesting set_field:\n");
    CHECK(set_field(0xB6C5u, 4, 4, 3u) == 0xB635u);
    CHECK(set_field(0xAAAAAAAAu, 0, 32, 0x12345678u)
          == 0x12345678u);
    CHECK(set_field(0x12345678u, 31, 2, 3u)
          == 0x12345678u);

    printf("\nTesting sign_extend:\n");
    CHECK(sign_extend(0xF8u, 8) == -8);
    CHECK(sign_extend(0x7Fu, 8) == 127);
    CHECK(sign_extend(0x80u, 8) == -128);
    CHECK(sign_extend(0u, 1) == 0);
    CHECK(sign_extend(1u, 1) == -1);
    CHECK(sign_extend(0xFFFFFFFFu, 32) == -1);

    printf("\n%d test(s) failed\n", fails);

    return fails != 0;
}
