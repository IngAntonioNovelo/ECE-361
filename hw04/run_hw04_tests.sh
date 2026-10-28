#!/usr/bin/env bash
# Run this script from the HW04 root: bash run_hw04_tests.sh
# Does not overwrite tally.c, tally.h, events.c, or Makefile.
set -u

cd "$(dirname "${BASH_SOURCE[0]}")" || exit 2

for file in tally.c tally.h events.c tests/test_hw04_tally.c tests/test_hw04_events.sh; do
	if [[ ! -f "$file" ]]; then
		printf 'Missing required file: %s\n' "$file" >&2
		exit 2
	fi
done

BUILDDIR=$(mktemp -d)
trap 'rm -rf "$BUILDDIR"' EXIT
CC=${CC:-gcc}

printf '== Compile tally ADT tests ==\n'
if ! "$CC" -std=c11 -Wall -Wextra -Werror -g -I. \
	tally.c tests/test_hw04_tally.c -o "$BUILDDIR/test_hw04_tally"; then
	printf 'FAIL: tally ADT test compilation\n' >&2
	exit 1
fi

printf '\n== Run tally ADT tests ==\n'
adt_rc=0
"$BUILDDIR/test_hw04_tally" || adt_rc=1

printf '\n== Verify tally_t is opaque outside tally.c ==\n'
cat > "$BUILDDIR/opaque_negative.c" <<'INNER'
#include "tally.h"
int read_private_field(tally_t *t)
{
	return t->n;
}
INNER
opaque_rc=0
if "$CC" -std=c11 -Wall -Wextra -Werror -I. -c \
	"$BUILDDIR/opaque_negative.c" -o "$BUILDDIR/opaque_negative.o" \
	> "$BUILDDIR/opaque_compile.log" 2>&1; then
	printf 'FAIL: external code can access t->n (tally_t is not opaque)\n'
	opaque_rc=1
else
	printf 'PASS: external code cannot access t->n\n'
fi

printf '\n== Compile events ==\n'
if ! "$CC" -std=c11 -Wall -Wextra -Werror -g -I. \
	events.c tally.c -o "$BUILDDIR/events"; then
	printf 'FAIL: events program compilation\n' >&2
	exit 1
fi

printf '\n== Run black-box events tests ==\n'
events_rc=0
bash tests/test_hw04_events.sh "$BUILDDIR/events" || events_rc=1

printf '\n==================================\n'
if ((adt_rc == 0 && opaque_rc == 0 && events_rc == 0)); then
	printf 'OVERALL RESULT: PASS\n'
	exit 0
else
	printf 'OVERALL RESULT: FAIL\n'
	printf 'Inspect FAIL lines above and share them for targeted fixes.\n'
	exit 1
fi
