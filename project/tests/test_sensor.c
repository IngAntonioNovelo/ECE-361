
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "sensor.h"
#include "iom361_r4.h"

static int close_float(float a, float b)
{
	return fabsf(a - b) < 0.001f;
}

int main(void)
{
	int rc;
	uint32_t *base = iom361_initialize(1, 2, &rc);
	reading_t reading;

	assert(base != NULL);
	assert(rc == 0);

	iom361_set_display(false);

	/* Test 0 degrees Celsius. */
	_iom361_set_sensor1(0.0f, 50.0f);

	assert(sensor_read(base, 100, &reading) == 0);
	assert(reading.timestamp == 100);
	assert(close_float(reading.temp_c, 0.0f));
	assert(close_float(reading.rh_pct, 50.0f));

	/* Test 100 degrees Celsius. */
	_iom361_set_sensor1(100.0f, 75.0f);

	assert(sensor_read(base, 101, &reading) == 0);
	assert(close_float(reading.temp_c, 100.0f));
	assert(close_float(reading.rh_pct, 75.0f));

	/* Test the exact heating/cooling stop threshold. */
	_iom361_set_sensor1(21.5f, 45.0f);

	assert(sensor_read(base, 102, &reading) == 0);
	assert(close_float(reading.temp_c, 21.5f));
	assert(close_float(reading.rh_pct, 45.0f));

	/* Failure must preserve the existing reading. */
	reading_t before = reading;

	assert(sensor_read(NULL, 103, &reading) != 0);

	assert(reading.timestamp == before.timestamp);
	assert(reading.temp_c == before.temp_c);
	assert(reading.rh_pct == before.rh_pct);

	/* An unrecognized register base should fail. */
	uint32_t invalid_base[8] = {0};

	assert(sensor_read(invalid_base, 104, &reading) != 0);

	assert(reading.timestamp == before.timestamp);
	assert(reading.temp_c == before.temp_c);
	assert(reading.rh_pct == before.rh_pct);

	puts("PASS: sensor conversion and error handling");

	return 0;
}
