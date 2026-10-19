#include <stddef.h>
#include "ptr.h"

void swap_float(float *a, float *b)
{
	float temp = *a;
	*a = *b;
	*b = temp;
}

int minmax(const float *a, int n, float *min, float *max)
{
	if (n < 1 || a == NULL || min == NULL || max == NULL)
	{
		return -1;
	}

	*min = *a;
	*max = *a;

	for (const float *p = a + 1; p <  a + n; p++)
	{
		if (*min > *p)
		{
			*min = *p;
		}

		if (*max < *p)
		{
			*max = *p;
		}
	}
	return 0;
}

const float *find_above(const float *a, int n, float threshold)
{
	if (n < 1 || a == NULL)
	{
		return NULL;
	}
	for(const float *p = a; p < a + n; p++)
		{
			if(*p > threshold)
			{
				return p;
			}
		}
	return NULL;
}


void reverse(float *a, int n)
{
	if (n <= 1 || a == NULL)
	{
		return;
	}
	float *lo = a;
	float *hi = a+ n - 1;
	while (lo < hi)
	{
		swap_float(lo, hi);
		lo++;
		hi--;
	}
}
