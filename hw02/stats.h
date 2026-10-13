#ifndef STATS_H
#define STATS_H
/* Minimum of n samples; assumes n >= 1. */
float stats_min(const float a[], int n);
/* Maximum of n samples, recursively; assumes n >= 1. */
float stats_max(const float a[], int n);
/* Arithmetic mean of n samples; assumes n >= 1. */
float stats_mean(const float a[], int n);
/* Longest consecutive run strictly above threshold.
   Returns run length; writes index of best run to best_start (-1 if none).
   Assumes n >= 0 and best_start is a valid pointer. */
int stats_longest_run_above(const float a[], int n, float threshold, int *best_start);
#endif
