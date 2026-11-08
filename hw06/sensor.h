#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <time.h>

typedef struct 
{
    time_t t; // timestamp of the reading
    float temp_c; // temperature in Celsius
    float rh_pct; // relative humidity percentage

}reading_t;

int sensor_read(uint32_t *base, time_t now, reading_t *r); // read the sensor and fill the reading_t structure

#endif
