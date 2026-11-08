#include "panel.h"
#include "iom361_r4.h"

uint32_t panel_pressed(uint32_t now, uint32_t before) // return the bits that are set in now but not in before (i.e. the buttons that were just pressed)
{
    return (now & ~before);

}

uint32_t panel_released(uint32_t now, uint32_t before) // return the bits that are set in before but not in now (i.e. the buttons that were just released)
{
    return (~now & before);
}

int panel_led(uint32_t *base, int n, int on) // turn on/off the n-th LED
{
    if (n < 0 || n > 31)
    {
        return -1; // invalid LED number
    }

    int rc = 0; // Variable to hold the return code from the read operation

    uint32_t leds = iom361_readReg(base, LEDS_REG, &rc); // Read the current state of the LEDs
    
    if (rc != 0)
    {
        return rc; // Return the error code if reading failed
    }

    if (on)
    {
        leds |= (1u << n); // Set the n-th bit to turn on the LED
    }
    else
    {
        leds &= ~(1u << n); // Clear the n-th bit to turn off the LED
    }

    iom361_writeReg(base, LEDS_REG, leds, &rc); // Write the new state of the LEDs back to the register

    return rc; // Return the result of the write operation (0 for success, non-zero for error)
}

int panel_toggle(uint32_t *base, int n) // toggle the n-th LED
{
    if (n < 0 || n > 31)
    {
        return -1; // invalid LED number
    }

    int rc = 0; // Variable to hold the return code from the read operation

    uint32_t leds = iom361_readReg(base, LEDS_REG, &rc); // Read the current state of the LEDs
    
    if (rc != 0)
    {
        return rc; // Return the error code if reading failed
    }

    leds ^= (1u << n); // Toggle the n-th bit to change the state of the LED

    iom361_writeReg(base, LEDS_REG, leds, &rc); // Write the new state of the LEDs back to the register

    return rc; // Return the result of the write operation (0 for success, non-zero for error)
}