
#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <time.h>

/* One timestamped sensor reading. */
typedef struct {
	time_t timestamp;
	float temp_c;
	float rh_pct;
} reading_t;

/*
 * Read temperature and humidity from iom361.
 *
 * Returns 0 on success, -1 on failure.
 * On failure, *out is left unchanged.
 */
int sensor_read(
	uint32_t *base,
	time_t timestamp,
	reading_t *out
);

#endif
