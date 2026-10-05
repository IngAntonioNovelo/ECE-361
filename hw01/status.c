#include "status.h"
#include "bits.h"

enum 
{
	HEAT_POS = 0,
	HEAT_WIDTH = 1,

	COOL_POS = 1,
	COOL_WIDTH = 1,

	FAN_POS = 2,
	FAN_WIDTH = 1,

	FAULT_POS = 3,
	FAULT_WIDTH = 1,

	MODE_POS = 4,
	MODE_WIDTH = 3,

	RESERVED_POS = 7,
	RESERVED_WIDTH = 1,

	SETPOINT_POS = 8,
	SETPOINT_WIDTH = 8
};

status_t status_unpack(uint16_t word)
{
	status_t status;

	status.heat = (uint8_t)get_field(word, HEAT_POS, HEAT_WIDTH);

	status.cool = (uint8_t)get_field(word, COOL_POS, COOL_WIDTH);

	status.fan = (uint8_t)get_field(word, FAN_POS, FAN_WIDTH);

	status.fault = (uint8_t)get_field(word, FAULT_POS, FAULT_WIDTH);

	status.mode = (uint8_t)get_field(word, MODE_POS, MODE_WIDTH);

	status.reserved = (uint8_t)get_field(word, RESERVED_POS, RESERVED_WIDTH);

	status.setpoint = sign_extend(get_field(word, SETPOINT_POS, SETPOINT_WIDTH), SETPOINT_WIDTH);

	return status;
}
