#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>

uint32_t panel_pressed(uint32_t now, uint32_t before); // return the bits that are set in now but not in before (i.e. the buttons that were just pressed)
uint32_t panel_released(uint32_t now, uint32_t before); // return the bits that are set in before but not in now (i.e. the buttons that were just released)

int panel_led(uint32_t *base, int n, int on); // turn on/off the n-th LED
int panel_toggle(uint32_t *base, int n); // toggle the n-th LED

#endif