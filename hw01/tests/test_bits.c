#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "bits.h"
#include "status.h"

static int fails = 0;

#define CHECK(cond) do { \
	if (cond) \
	{ \
		printf("PASS: %s\n", #cond); \
	} \
	else \
	{ \
		printf("FAIL: %s (line %d)\n", #cond, __LINE__); \
		fails++; \
	} \
} while (0)

static int print_binary_matches(uint32_t x, int width, const char *expected)
{
	int saved_stdout = dup(fileno(stdout));

	if (saved_stdout == -1)
	{
		return 0;
	}

	FILE *tmp = tmpfile();

	if (tmp == NULL)
	{
		close(saved_stdout);
		return 0;
	}

	fflush(stdout);

	if (dup2(fileno(tmp), fileno(stdout)) == -1)
	{
		fclose(tmp);
		close(saved_stdout);
		return 0;
	}

	print_binary(x, width);
	fflush(stdout);

	if (dup2(saved_stdout, fileno(stdout)) == -1)
	{
		fclose(tmp);
		close(saved_stdout);
		return 0;
	}

	close(saved_stdout);
	rewind(tmp);

	char buffer[128];
	size_t count = fread(buffer, 1, sizeof(buffer) - 1, tmp);

	buffer[count] = '\0';

	fclose(tmp);

	return strcmp(buffer, expected) == 0;
}

int main(void)
{
	status_t status;

	printf("Testing print_binary:\n");

	CHECK(print_binary_matches(0x2Cu, 8, "0010 1100"));
	CHECK(print_binary_matches(1u, 1, "1"));
	CHECK(print_binary_matches(
		0xFFFFFFFFu,
		32,
		"1111 1111 1111 1111 1111 1111 1111 1111"
	));

	printf("\nTesting get_field:\n");

	CHECK(get_field(0x80000000u, 31, 1) == 1u);
	CHECK(get_field(0x12345678u, 0, 32) == 0x12345678u);
	CHECK(get_field(0x12345678u, 31, 2) == 0u);

	printf("\nTesting set_field:\n");

	CHECK(set_field(0u, 31, 1, 1u) == 0x80000000u);
	CHECK(set_field(0u, 0, 32, 0x12345678u) == 0x12345678u);
	CHECK(set_field(0u, 4, 4, 0xABu) == 0xB0u);
	CHECK(
		set_field(0x12345678u, 31, 2, 3u)
		== 0x12345678u
	);

	printf("\nTesting sign_extend:\n");

	CHECK(sign_extend(0u, 1) == 0);
	CHECK(sign_extend(1u, 1) == -1);
	CHECK(sign_extend(0x80u, 8) == -128);
	CHECK(sign_extend(0x80000000u, 32) == INT32_MIN);
	CHECK(sign_extend(0x7FFFFFFFu, 32) == INT32_MAX);

	printf("\nTesting status_unpack:\n");

	status = status_unpack(0x1631u);

	CHECK(status.heat == 1u);
	CHECK(status.cool == 0u);
	CHECK(status.fan == 0u);
	CHECK(status.fault == 0u);
	CHECK(status.mode == 3u);
	CHECK(status.reserved == 0u);
	CHECK(status.setpoint == 22);

	status = status_unpack(0xF819u);

	CHECK(status.heat == 1u);
	CHECK(status.fault == 1u);
	CHECK(status.mode == 1u);
	CHECK(status.setpoint == -8);

	status = status_unpack(0x8050u);

	CHECK(status.heat == 0u);
	CHECK(status.cool == 0u);
	CHECK(status.fan == 0u);
	CHECK(status.fault == 0u);
	CHECK(status.mode == 5u);
	CHECK(status.reserved == 0u);
	CHECK(status.setpoint == -128);

	printf("\n%d test(s) failed\n", fails);

	return fails != 0;
}
