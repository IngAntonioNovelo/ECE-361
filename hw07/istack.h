
#ifndef ISTACK_H
#define ISTACK_H

#include <stdbool.h>
#include <stddef.h>

#define ISTACK_START 8

typedef struct istack istack_t;

/* Create an empty stack. Returns NULL on failure. */
istack_t *istack_create(void);

/* Free the stack and its contents. NULL is allowed. */
void istack_destroy(istack_t *s);

/* Push v. Returns 0 on success, -1 on failure. */
int istack_push(istack_t *s, int v);

/* Remove the top. Copy it to v if v is non-NULL. */
int istack_pop(istack_t *s, int *v);

/* Read the top without removing it. */
int istack_peek(const istack_t *s, int *v);

/* Return the number of stored values; 0 for NULL. */
size_t istack_count(const istack_t *s);

/* Return whether the stack is empty; true for NULL. */
bool istack_is_empty(const istack_t *s);

/* Return current capacity; 0 for NULL. */
size_t istack_capacity(const istack_t *s);

/* Return a heap-allocated bottom-first copy.
   Set *n to its length when n is non-NULL.
   Caller must free the returned array. */
int *istack_to_array(const istack_t *s, size_t *n);

#endif
