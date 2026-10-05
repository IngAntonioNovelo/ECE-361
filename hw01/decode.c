#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bits.h"
#include "status.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <hex word>\n", argv[0]);
		return 1;
	}

	char *end;
	errno = 0;

	unsigned long value = strtoul(argv[1], &end, 16);

	if (errno != 0 ||
		end == argv[1] ||
		*end != '\0' ||
		value > 0xFFFFu)
	{
		fprintf(stderr, "error: invalid 16-bit hexadecimal word\n");
		return 1;
	}

	uint32_t word = (uint32_t)value;

	status_t status = status_unpack((uint16_t)value);

	printf("word: 0x%04lX\n", value);

	printf("binary: ");
	print_binary(word, 16);
	putchar('\n');

	printf("HEAT: %s\n", status.heat ? "on" : "off");
	printf("COOL: %s\n", status.cool ? "on" : "off");
	printf("FAN: %s\n", status.fan ? "on" : "off");
	printf("FAULT: %s\n", status.fault ? "on" : "off");

	switch (status.mode)
	{
		case 0:
			printf("MODE: OFF\n");
			break;

		case 1:
			printf("MODE: HEAT\n");
			break;

		case 2:
			printf("MODE: COOL\n");
			break;

		case 3:
			printf("MODE: AUTO\n");
			break;

		case 4:
			printf("MODE: FAN_ONLY\n");
			break;

		default:
			printf("MODE: INVALID (%u)\n", (unsigned)status.mode);
			break;
	}

	printf("SETPOINT: %d C\n", status.setpoint);

	if (status.reserved)
	{
		fprintf(stderr, "warning: reserved bit 7 is set\n");
	}

	return 0;
}
