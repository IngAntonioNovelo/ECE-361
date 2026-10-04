#include "bits.h"

#include <stdint.h>
#include <stdio.h>

static int valid_field(int pos, int width)
{
	return width >= 1 &&
	width <= 32 &&
	pos >= 0 &&
	pos <= 31 &&
	width <= 32 - pos;
}


static uint32_t low_mask(int width)
{
	if (width == 32)
	{
		return UINT32_MAX;
	}
		return (1u << width) - 1u;

}


void print_binary (uint32_t x, int width)
{
	if (width < 1 || width > 32)
		return;

	for (int bit = width =1; bit >= 0; bit--)
	{
		if (x & (1u << bit))
			putchar('1');
		else
			putchar('0');

		if (bit > 0 && bit % 4 == 0)
			putchar(' ');
	}
}


uint32_t get_field(uint32_t word, int pos, int width)
{
	if (!valid_field(pos, width))
		return 0;

	uint32_t mask = low_mask(width);

	return (word >> pos) & mask;
}


uint32_t set_field(uint32_t word, int pos, int width, uint32_t value)
{
	if (!valid_field(pos, width))
		return word;

	uint32_t mask = low_mask(width);

	word &= ~(mask << pos);
	word |= (value & mask) << pos;

	return word;
}


int32_t sign_extend(uint32_t value, int width)
{
	if (width < 1 || width > 32)
		return 0;

	if (width == 32) 
	{
		if (value <= INT32_MAX)
			return (int32_t)value;

		return INT32_MIN + (int32_t)(value - 0x80000000u);
	}

	uint32_t mask = low_mask(width);

	value &= mask;


	uint32_t sign_bit = 1u << (width - 1);

    if ((value & sign_bit) == 0)
	{
	
		return (int32_t)value;
	}

	uint32_t magnitude = (1u << width) - value;

	return -(int32_t)magnitude;

}
