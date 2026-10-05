#ifndef STATUS_H
#define STATUS_H

#include <stdint.h>

typedef struct
{
	uint8_t heat;
	uint8_t cool;
	uint8_t fan;
	uint8_t fault;
	uint8_t mode;
	uint8_t reserved;
	int32_t setpoint;
} status_t;

status_t status_unpack(uint16_t word);

#endif
