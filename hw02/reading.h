#ifndef READING_H
#define READING_H
#include <stdio.h>
#define MAX_READINGS 1000
/* Read up to capacity samples from in; write skipped-line count to skipped.
   Arrays must each hold capacity values; returns number of valid readings. */
int reading_read(FILE *in, int ticks[], float temps[], float hums[], int capacity, int *skipped);
#endif
