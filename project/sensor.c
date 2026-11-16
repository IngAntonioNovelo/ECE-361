
#include "sensor.h"
#include "iom361_r4.h"

#include <math.h>
#include <stddef.h>

/* Convert raw iom361 values into a reading. */
int sensor_read(
	uint32_t *base,
	time_t timestamp,
	reading_t *out)
{
	int rc;
	uint32_t raw_temp;
	uint32_t raw_humid;
	reading_t next;

	/* Validate pointers before accessing memory. */
	if (base == NULL || out == NULL)
	{
		return -1;
	}

	/* Read temperature register. */
	raw_temp = iom361_readReg(base, TEMP_REG, &rc);

	if (rc != 0)
	{
		return -1;
	}

	/* Read humidity register. */
	raw_humid = iom361_readReg(base, HUMID_REG, &rc);

	if (rc != 0)
	{
		return -1;
	}

	/* Convert and round to one decimal place. */
	double temp_c =
		(double)raw_temp / 1048576.0 * 200.0 - 50.0;

	double rh_pct =
		(double)raw_humid / 1048576.0 * 100.0;

	next.timestamp = timestamp;
	next.temp_c = (float)(round(temp_c * 10.0) / 10.0);
	next.rh_pct = (float)(round(rh_pct * 10.0) / 10.0);

	/* Only update output after both reads succeed. */
	*out = next;

	return 0;
}
