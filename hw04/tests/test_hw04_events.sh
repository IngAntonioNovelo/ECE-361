#!/usr/bin/env bash
# Extra black-box tests for ECE 361 HW04 events.c.
# Pass path to compiled events executable as first argument.
set -u

if [[ $# -ne 1 ]]; then
	printf 'Usage: bash tests/test_hw04_events.sh /path/to/events\n' >&2
	exit 2
fi
EVENTS="$1"
if [[ ! -x "$EVENTS" ]]; then
	printf 'Executable not found: %s\n' "$EVENTS" >&2
	exit 2
fi

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
pass=0
fail=0

check_case() {
	local name="$1"
	if "$EVENTS" < "$TMP/input" > "$TMP/actual"; then
		if diff -u "$TMP/expected" "$TMP/actual"; then
			printf 'PASS: %s\n' "$name"
			pass=$((pass + 1))
		else
			printf 'FAIL: %s (output differs)\n' "$name"
			fail=$((fail + 1))
		fi
	else
		printf 'FAIL: %s (program returned nonzero exit code)\n' "$name"
		fail=$((fail + 1))
	fi
}

# 1. Empty input.
: > "$TMP/input"
printf 'names: 0, lines skipped: 0\n' > "$TMP/expected"
check_case 'empty input'

# 2. Mixed events, blank lines and comments.
printf 'TEMP\nBRAKE\n# comment\n\nBRAKE\nALARM\n' > "$TMP/input"
printf 'ALARM 1\nBRAKE 2\nTEMP 1\nnames: 3, lines skipped: 0\n' > "$TMP/expected"
check_case 'sorting, repetitions, comments and blank lines'

# 3. Fifteen characters accepted.
printf 'ABCDEFGHIJKLMNO\n' > "$TMP/input"
printf 'ABCDEFGHIJKLMNO 1\nnames: 1, lines skipped: 0\n' > "$TMP/expected"
check_case '15-character name accepted'

# 4. Sixteen characters rejected, rest of input handled.
printf 'BRAKE\nABCDEFGHIJKLMNOP\nTEMP\n' > "$TMP/input"
printf 'BRAKE 1\nTEMP 1\nnames: 2, lines skipped: 1\n' > "$TMP/expected"
check_case '16-character name rejected once'

# 5. 140 characters, followed by a valid event.
{
	printf 'BRAKE\n'
	printf 'A%.0s' {1..140}
	printf '\nTEMP\n'
} > "$TMP/input"
printf 'BRAKE 1\nTEMP 1\nnames: 2, lines skipped: 1\n' > "$TMP/expected"
check_case '140-character event discarded as one line'

# 6. 141-character comment, followed by a valid event.
{
	printf '#'
	printf 'A%.0s' {1..140}
	printf '\nBRAKE\n'
} > "$TMP/input"
printf 'BRAKE 1\nnames: 1, lines skipped: 0\n' > "$TMP/expected"
check_case 'long comment ignored, remainder discarded'

# 7. Final line with no newline, shorter than buffer.
printf 'BRAKE\nTEMP' > "$TMP/input"
printf 'BRAKE 1\nTEMP 1\nnames: 2, lines skipped: 0\n' > "$TMP/expected"
check_case 'final valid name without newline'

# 8. Exactly 127 characters plus newline, followed by valid event.
{
	printf 'A%.0s' {1..127}
	printf '\nBRAKE\n'
} > "$TMP/input"
printf 'BRAKE 1\nnames: 1, lines skipped: 1\n' > "$TMP/expected"
check_case '127-character full buffer boundary'

# 9. Exactly 128 characters plus newline.
{
	printf 'A%.0s' {1..128}
	printf '\nBRAKE\n'
} > "$TMP/input"
printf 'BRAKE 1\nnames: 1, lines skipped: 1\n' > "$TMP/expected"
check_case '128-character split input boundary'

# 10. Long event ending at EOF, no newline.
printf 'A%.0s' {1..140} > "$TMP/input"
printf 'names: 0, lines skipped: 1\n' > "$TMP/expected"
check_case 'long event without final newline'

# 11. Capacity limit: 16 names, reject 17th, increment existing.
{
	for i in $(seq -w 1 16); do
		printf 'EVENT%s\n' "$i"
	done
	printf 'EVENT17\nEVENT01\n'
} > "$TMP/input"
{
	printf 'EVENT01 2\n'
	for i in $(seq -w 2 16); do
		printf 'EVENT%s 1\n' "$i"
	done
	printf 'names: 16, lines skipped: 1\n'
} > "$TMP/expected"
check_case 'full tally and existing-name increment'

# 12. Longer-than-name-limit short comment isn't rejected.
printf '#ABCDEFGHIJKLMNOP\nBRAKE\n' > "$TMP/input"
printf 'BRAKE 1\nnames: 1, lines skipped: 0\n' > "$TMP/expected"
check_case 'comment longer than TALLY_LEN'

# 13. End-to-end mixed case with no final newline.
{
	printf 'TEMP\nBRAKE\n# Normal comment\n\nBRAKE\nALARM\nABCDEFGHIJKLMNOP\n'
	printf '#'
	printf 'A%.0s' {1..140}
	printf '\n'
	printf 'A%.0s' {1..140}
	printf '\nBRAKE\nTEMP'
} > "$TMP/input"
printf 'ALARM 1\nBRAKE 3\nTEMP 2\nnames: 3, lines skipped: 2\n' > "$TMP/expected"
check_case 'complex mixed input, EOF without newline'

printf '\nEVENTS SUMMARY: %d passed, %d failed\n' "$pass" "$fail"
((fail == 0))
