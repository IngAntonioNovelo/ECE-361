#include "sensor.h"
#include "iom361_r4.h"
#include <math.h>

int sensor_read(uint32_t *base, time_t now, reading_t *r)
{
    int rc = 0; // Variable to hold the return code from the read operation

    uint32_t raw_temp = iom361_readReg(base, TEMP_REG, &rc); // Read the raw temperature value from the sensor register
    
    // Check if the read operation was successful
    if (rc != 0)
    {
        return rc; // Return the error code if reading failed
    }

    uint32_t raw_humid = iom361_readReg(base, HUMID_REG, &rc); // Read the raw humidity value from the sensor register

    // Check if the read operation was successful
    if (rc != 0)
    {
        return rc; // Return the error code if reading failed
    }

    raw_temp &= 0xFFFFFu; // Mask the raw temperature to 20 bits
    raw_humid &= 0xFFFFFu; // Mask the raw humidity to 20 bits

    double temp = raw_temp / 1048576.0 * 200.0 - 50.0; // Convert raw temperature to Celsius
    double rounded_temp = lround(temp *10.0) / 10.0; // Round the temperature to one decimal place
    double rh = raw_humid / 1048576.0 * 100.0; // Convert raw humidity to percentage
    double rounded_rh = lround(rh *10.0) / 10.0; // Round the humidity to one decimal place
    
    r->t = now;
    r->temp_c = rounded_temp; // Store the rounded temperature in the reading structure
    r->rh_pct = rounded_rh; // Store the rounded humidity in the reading structure
    
    return 0;
}