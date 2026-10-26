#ifndef TALLY_H
#define TALLY_H
#include <stdio.h>

#define TALLY_MAX 4	/* tallies that can exist at the same time */
#define TALLY_NAMES 16	/* different names one tally can hold */
#define TALLY_LEN 15	/* longest name, without the '\0' */

typedef struct tally tally_t;	/* opaque */

// a new, empty tally; NULL if TALLY_MAX tallies are already in use
tally_t *tally_create(void);

// gives the tally back so that tally_create can hand it out again; NULL does nothing
void tally_destroy(tally_t *t);

// counts one more occurrence of name and returns its new count; returns −1 and changes
// nothing if t or name is NULL, name is empty or longer than TALLY_LEN, or name is new and
// the tally is full
int tally_add(tally_t *t, const char *name);

// how often name was added: 0 if never, or for NULL
int tally_count(const tally_t *t, const char *name);

// how many different names the tally holds: 0 for NULL
int tally_size(const tally_t *t);

// one line per name, name count, in alphabetical (strcmp) order
void tally_print(const tally_t *t, FILE *out);

#endif
