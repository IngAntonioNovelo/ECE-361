
#ifndef PTR_H
#define PTR_H

/* Takes: two pointers to float variables and returns nothing
 * Assumes: Both pointers refer to valid, writeable floats.
*/
void swap_float(float *a, float *b);

/* Takes: Three pointers to float variables and one integer
 * variable. Stores the smallest and largest of a[0..n-1] in
 * *min and *max and returns 0; returns -1 and changes nothing
 * if n < 1 or any pointer is NULL
 * Assumes: All pointers refer to valid, writeable floats.
*/

int minmax(const float *a, int n, float *min, float *max);

/* Takes: a constant float and two Returns a ponter to the first value greater than threshold, or 
 * NULL if there is none
*/

const float *find_above(const float *a, int n, float threshold);

/* Reverses a[0..n-1] in place; does nothing if a is NULL or n < 2
*/

void reverse(float *a, int n);

#endif
