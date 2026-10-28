# ECE 361 — Homework 04

## Overview

This project implements an event-counting program in C using an opaque abstract data type (ADT) called `tally_t`.

The tally stores event names and counts how many times each name appears. Names are maintained in alphabetical order, and the event-processing program reads input from standard input.

## Project Files

- `tally.h` — Public interface for the tally ADT.
- `tally.c` — Private structures and implementation of the tally functions.
- `events.c` — Reads input lines, adds events, and prints the results.
- `Makefile` — Builds the executable and runs the test suite.
- `tests/` — Contains unit and integration tests.
- `run_hw04_tests.sh` — Runs the automated test suite.
- `AI_USAGE.md` — Documents AI assistance.

## Tally ADT

The public interface provides six functions:

| Function | Purpose |
|---|---|
| `tally_create()` | Creates an empty tally from the static pool |
| `tally_destroy()` | Releases a tally for reuse |
| `tally_add()` | Adds a name or increments its count |
| `tally_count()` | Returns the occurrence count for a name |
| `tally_size()` | Returns the number of distinct names |
| `tally_print()` | Prints names and counts alphabetically |

The implementation supports up to four simultaneous tally objects, with a maximum of 16 distinct names per tally and 15 characters per name.

## Opaque Type Design

The public header declares `typedef struct tally tally_t;` without exposing the structure's internal members.

The complete structure definition is located in `tally.c`. This prevents programs that include only `tally.h` from directly accessing implementation details such as `t->n`.

For example, attempting to access `t->n` in `events.c` would fail to compile because the structure is incomplete in that translation unit.

This design provides encapsulation and requires users of the ADT to access its functionality through the public interface.

## Building and Running

From the `hw04` directory, run `make` to build the program.

Run `./events` to enter event names through standard input, or redirect a file using `./events < input.txt`.

Run `make test` to execute the automated tests, and `make clean` to remove generated build files.

## Input Handling

The program reads lines using `fgets()` and removes trailing newline characters.

Empty lines and lines beginning with `#` are ignored. Invalid event names and rejected additions increment the skipped counter.

When an input line exceeds the buffer, the remaining characters are discarded so that a single physical line cannot accidentally become multiple events.

## Example

Input:

    BRAKE
    TEMP
    BRAKE
    ALARM

Output:

    ALARM 1
    BRAKE 2
    TEMP 1
    names: 3, lines skipped: 0

## Testing

The automated suite was run using GCC with C11, compiler warnings enabled, and warnings treated as errors.

The latest test results were:

- Tally ADT: 40 passed, 0 failed.
- Events integration: 13 passed, 0 failed.
- Opaque-type protection: passed.

Tests included pool exhaustion and reuse, name-length limits, sorting, invalid inputs, repeated names, full-capacity behavior, long input lines, comments, empty input, and end-of-file handling.
