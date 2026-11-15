
#include "istack.h"
#include <stdint.h>
#include <stdlib.h>

/* Values are stored bottom-first; the top is at data[count - 1]. */
struct istack
{
	int *data;     /* Dynamically allocated storage for stack values. */
	size_t count;  /* Number of values currently stored. */
	size_t cap;    /* Number of values that fit in the allocated array. */
};


/* Allocate an empty stack and its initial array; return NULL on failure. */
istack_t *istack_create(void)
{
	istack_t *s = malloc(sizeof *s);

	if (s == NULL)
	{
		return NULL;
	}

	s->data = malloc(ISTACK_START * sizeof *s->data);

	if (s->data == NULL)
	{
		/* Release the structure if allocating its array fails. */
		free(s);
		return NULL;
	}

	s->count = 0;
	s->cap = ISTACK_START;

	return s;
}

/* Release the array before its owning structure; NULL needs no cleanup. */
void istack_destroy(istack_t *s)
{
	if (s == NULL)
	{
		return;
	}

	free(s->data);
	free(s);
}

/* Append a new top value, doubling storage when full. */
int istack_push(istack_t *s, int v)
{
	/* Step 1: Validate s */
    if (s == NULL)
    {
        return -1;
    }

    /* Step 2: Check whether count == cap */
    if (s->count == s->cap)
    {
        /* Check that doubling the capacity cannot overflow size_t. */
        if (s->cap > SIZE_MAX / 2)
        { 
            return -1;
        }

        
        /* Step 3: Grow array safely if full */
        size_t new_cap = s->cap * 2;
        
        /* Check that converting the new capacity to bytes is also safe. */
        if(new_cap > SIZE_MAX /sizeof *s->data)
        {
            return -1;
        }

        /* Keep the original allocation intact if realloc fails. */
        int *bigger = realloc(s->data,
            new_cap * sizeof *s->data);

        if (bigger == NULL)
        {
            return -1;
        }

        s->data = bigger;
        s->cap = new_cap;
    }

    /* Step 4: Store v in the next position */
    s->data[s->count] = v;
    
    /* Step 5: Increase count */
    s->count++;

    /* Step 6: Return success */
    return 0;	
}


/* Remove the top value; v may be NULL when the value is not needed. */
int istack_pop(istack_t *s, int *v)
{
	if (s == NULL || s->count == 0)
	{
		return -1;
	}

	/* After decrementing, count is the index of the removed top value. */
	s->count--;

	if (v != NULL)
	{
		*v = s->data[s->count];
	}

	return 0;
}

/* Read the top value without removing it; reject a NULL or empty stack. */
int istack_peek(const istack_t *s, int *v)
{
	if (s == NULL || s->count == 0)
	{
		return -1;
	}

	if (v != NULL)
	{
		*v = s->data[s->count - 1];
	}

	return 0;
}

/* Return the number of stored values, treating NULL as an empty stack. */
size_t istack_count(const istack_t *s)
{
	if (s == NULL)
	{
		return 0;
	}

	return s->count;
}

/* A NULL stack is considered empty. */
bool istack_is_empty(const istack_t *s)
{
	return s == NULL || s->count == 0;
}

/* Return the allocated capacity, or zero when there is no stack. */
size_t istack_capacity(const istack_t *s)
{
	if (s == NULL)
	{
		return 0;
	}

	return s->cap;
}


/* Return an independent bottom-first copy that the caller must free.
   If n is non-NULL, also report the number of copied values. */
int *istack_to_array(const istack_t *s, size_t *n)
{
	/* Initialize the optional length to zero for an empty stack or failure. */
	if (n != NULL)
	{
		*n = 0;
	}

	if (s == NULL || s->count == 0)
	{
		return NULL;
	}

	int *copy;

	/* Ensure the allocation size in bytes cannot overflow size_t. */
	if (s->count > SIZE_MAX / sizeof *copy)
	{
		return NULL;
	}

	/* Allocate separate storage so changes to the copy cannot alter the stack. */
	copy = malloc(s->count * sizeof *copy);

	if (copy == NULL)
	{
		return NULL;
	}

	/* Preserve stack order: bottom at index zero, top at the last index. */
	for (size_t i = 0; i < s->count; i++)
	{
		copy[i] = s->data[i];
	}

	/* Publish the optional length only after the entire copy is ready. */
	if (n != NULL)
	{
		*n = s->count;
	}

	return copy;
}
