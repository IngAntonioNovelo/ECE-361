#include "tally.h"

/* 
 * One entry holds an event name and the number of times that 
 * name has been added
*/

struct entry 
{
	char name [TALLY_LEN + 1];
	int count;
};

struct tally
{
	int in_use;
	int n;
	struct entry e[TALLY_NAMES];
};

static struct tally pool[TALLY_MAX];

tally_t *tally_create(void)
{
	for (int i = 0; i < TALLY_MAX; i++)
	{
		if (pool[i].in_use == 0)
		{
			pool[i].in_use = 1;
			pool[i].n = 0;

			return &pool[i];
		}


	}
	return NULL;
}

void tally_destroy(tally_t *t)
{
	if (t == NULL)
	{
		return;
	}
	t->in_use = 0;
}
