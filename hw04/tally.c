#include "tally.h"
#include <string.h>

/*
 * One entry holds an event name and the number
 * of times that name has been added.
 */
struct entry
{
	char name[TALLY_LEN + 1];
	int count;
};

/*
 * A tally tracks its allocation status,
 * number of distinct names, and entries.
 */
struct tally
{
	int in_use;
	int n;
	struct entry e[TALLY_NAMES];
};

/* Pool of available tally objects. */
static struct tally pool[TALLY_MAX];


/* Create a new, empty tally. */
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


/* Release a tally back into the pool. */
void tally_destroy(tally_t *t)
{
	if (t == NULL)
	{
		return;
	}

	t->in_use = 0;
}


/* Add one occurrence of a name to a tally. */
int tally_add(tally_t *t, const char *name)
{
	/* Check for NULL pointers. */
	if (t == NULL || name == NULL)
	{
		return -1;
	}

	/* Reject empty names. */
	if (name[0] == '\0')
	{
		return -1;
	}

	/* Check the length of the name. */
	size_t len = strlen(name);

	if (len > TALLY_LEN)
	{
		return -1;
	}

	/* Search for an existing name. */
	for (int i = 0; i < t->n; i++)
	{
		if (strcmp(name, t->e[i].name) == 0)
		{
			t->e[i].count++;
			return t->e[i].count;
		}
	}

	/* Reject new names if the tally is full. */
	if (t->n >= TALLY_NAMES)
	{
		return -1;
	}

	/* Find the alphabetical insertion position. */
	int pos = 0;

	while (pos < t->n &&
		strcmp(t->e[pos].name, name) < 0)
	{
		pos++;
	}

	/* Shift entries right to make room. */
	for (int k = t->n; k > pos; k--)
	{
		t->e[k] = t->e[k - 1];
	}

	/* Copy the new name, including '\0'. */
	memcpy(t->e[pos].name, name, len + 1);

	/* Initialize the new entry. */
	t->e[pos].count = 1;
	t->n++;

	/* Return the new occurrence count. */
	return t->e[pos].count;
}

// How often name was added: 0 if never, or for NULL
int tally_count(const tally_t *t, const char *name)
{
	/* State A: Handle NULL inputs.*/
	if (t == NULL || name == NULL)
	{
		return 0;
	}

	/* Searching for existing entry in array*/
	for (int i = 0; i < t->n; i++)
	{
		if (strcmp(name, t->e[i].name) == 0)
		{
			return t->e[i].count;
		}
	}

	/* No matching name was found. */
	return 0;
}

// how many different names the tally holds: 0 for NULL
int tally_size(const tally_t *t)
{
	if (t == NULL)
	{
		return 0;
	}

	/* State B: Return the number of distinct names. */
	return t->n;
}
