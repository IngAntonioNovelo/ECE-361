# ECE 361 — Homework 7: Dynamic Memory and the Stack ADT

## Overview

HW07 implements a dynamically allocated **stack of integers** (`istack`) and uses that stack to validate matching brackets in text read from standard input. The assignment practices `malloc()`, `realloc()`, `free()`, pointer ownership, error handling, testing, and memory sanitizers.

The stack follows **last in, first out (LIFO)** order. Its initial capacity is eight integers; when full, capacity doubles. Its allocated capacity never shrinks when values are removed.

The bracket checker handles `()`, `[]`, and `{}`. It reads every input character, including brackets in comments or quoted strings, and stops at the first bracket error.

## Files

| Path | Purpose |
| --- | --- |
| `istack.h` | Public interface for the opaque integer-stack ADT |
| `istack.c` | Dynamic array, stack operations, and memory management |
| `balance.c` | Command-line bracket checker using `istack` |
| `tests/test_istack.c` | Unit tests for stack behavior and edge cases |
| `inputs/*.txt` | Regression-test inputs for `balance` |
| `expected/*.out` | Exact expected output for each regression case |
| `expected/*.status` | Expected process exit status for each case |
| `regress.sh` | Regression runner comparing output and exit status |
| `Makefile` | Build, test, sanitizer, and cleanup targets |
| `README.md` | Project instructions and memory-error experiments |
| `AI_USAGE.md` | Disclosure of AI assistance |

## Build and run

Requirements: a C11 compiler (tested with GCC in WSL Ubuntu), `make`, and a POSIX-compatible shell.

Run all commands from the `hw07/` directory.

```sh
make
```

This builds the `balance` executable using compiler warnings and errors enabled.

Run the checker by redirecting a file into standard input:

```sh
./balance < inputs/balanced.txt
```

Or supply text directly:

```sh
printf '([{}])' | ./balance
# balanced: 3 pairs, deepest 3
```

An unbalanced example:

```sh
printf '([)]' | ./balance
# line 1, column 3: found ')', expected ']'
```

The program prints exactly one result line for a completed bracket check. It returns **0** if balanced and **1** for an unmatched/mismatched bracket or an incomplete input. Lines and columns are counted starting at 1. The `deepest` field is the maximum number of simultaneously open brackets, not the final stack size.

## Stack interface and behavior

The stack's structure is defined privately in `istack.c`, so callers access it through `istack_t *` and the public functions in `istack.h`.

| Function | Behavior |
| --- | --- |
| `istack_create()` | Allocates an empty stack with capacity 8; returns `NULL` on allocation failure |
| `istack_destroy(s)` | Frees the internal array and then the stack structure; accepts `NULL` |
| `istack_push(s, v)` | Pushes `v`; doubles capacity using `realloc()` if needed; returns 0 on success, -1 on failure |
| `istack_pop(s, v)` | Removes the top value and optionally copies it to `*v`; returns -1 on NULL/empty stacks |
| `istack_peek(s, v)` | Optionally copies the top value to `*v` without removing it; returns -1 on NULL/empty stacks |
| `istack_count(s)` | Returns the number of stored values (0 if `s == NULL`) |
| `istack_is_empty(s)` | Returns whether the stack is empty (`true` for `NULL`) |
| `istack_capacity(s)` | Returns allocated capacity (0 if `s == NULL`) |
| `istack_to_array(s, n)` | Returns a separately allocated, bottom-first copy; optionally writes its length through `n` |

The stack stores integers in an array with the top at index `count - 1`. A ninth push changes capacity from 8 to 16; 1,000 pushes grow it to 1,024. Popping values decreases `count` but does not reduce `capacity`.

To protect existing data on allocation failure, `istack_push()` stores `realloc()`'s return value in a temporary pointer and updates `s->data` only if it succeeds. It also checks for overflow when calculating the new allocation size. Failed pops or peeks do not change the caller's output value. The caller owns any non-NULL array returned by `istack_to_array()` and must release it with `free()`.

## Bracket checker logic

1. Allocate a stack to hold opening-bracket character codes.
2. Read standard input with `getchar()`, keeping track of line and column.
3. Push each opening bracket `(`, `[`, or `{` and update maximum depth.
4. For each closing bracket `)`, `]`, or `}`, inspect the most recent opener with `istack_peek()`.
5. If there is no opener or the closer does not match, print the first error, destroy the stack, and exit with status 1.
6. If the brackets match, pop the opener and increment the completed-pair counter.
7. At end of input, report any remaining unmatched opening brackets. Otherwise, report total pairs and deepest nesting.
8. Destroy the stack on every exit path.

## Tests

```sh
make test
```

This runs the stack unit tests followed by `regress.sh`.

**Stack unit tests** cover NULL inputs, empty operations and unchanged output parameters, LIFO order, peeking without removal, optional output pointers, negative and zero values, reuse of a drained stack, filling capacity 8, doubling to 16 on the ninth push, preservation of values, a 1,000-element push/pop sequence, independent stack instances, copying values bottom-first without sharing storage, and destroying nonempty stacks.

**Bracket regression tests** use these six input cases:

| Case | Example input | Expected result | Status |
| --- | --- | --- | --- |
| `balanced` | `([{}])` | `balanced: 3 pairs, deepest 3` | 0 |
| `deep` | Nine nested parentheses pairs | `balanced: 9 pairs, deepest 9` | 0 |
| `empty` | Empty file | `balanced: 0 pairs, deepest 0` | 0 |
| `mismatch` | `([)]` | `line 1, column 3: found ')', expected ']'` | 1 |
| `nothing_open` | `())` | `line 1, column 3: found ')', nothing open` | 1 |
| `unclosed` | `([` | `end of input: 2 still open, innermost '['` | 1 |

`regress.sh` compares the program's output with `.out` files and its process status with `.status` files. It prints PASS/FAIL per case and returns a nonzero exit status on any failure.

**Recorded passing results from the local WSL run:**

```text
Summary: 27 passed, 0 failed
Regression summary: 6 passed, 0 failed
```

## Memory checks and cleanup

```sh
make memcheck
```

This cleans and rebuilds the test programs with `-fsanitize=address,undefined` and runs the unit and regression tests. On Linux, LeakSanitizer can also report unreleased heap allocations. The recorded clean run produced the same 27 unit-test passes and six regression-test passes with no sanitizer errors.

```sh
make clean
```

This removes generated executables. It does not delete the C source code or test input/expected files.

## Part 4 — Intentional memory errors

Each experiment below was introduced **temporarily** and tested with `make memcheck`. The original working implementation must be restored before submission. The error lines are copied from the actual WSL AddressSanitizer/LeakSanitizer reports; process IDs and memory addresses are run-specific.

### Experiment 1: Destroy the same stack twice

**Modification:** Call `istack_destroy(demo)` twice using the same pointer.

**First sanitizer error:**

```text
==57487==ERROR: AddressSanitizer: heap-use-after-free on address 0x7a1b60fe0040 at pc 0x65521cf4d1c8 bp 0x7ffd5cf0c980 sp 0x7ffd5cf0c970
```

**What happened in memory:** The first destroy call freed the stack's array and structure, but `demo` remained a dangling pointer; the second call accessed `s->data` through the already-freed structure, so the sanitizer caught a heap-use-after-free before a second `free()` could execute.

**Correction:** Remove the repeated destroy call and rerun `make memcheck`.

### Experiment 2: Push onto a destroyed stack

**Modification:** Call `istack_push(demo, 42)` after `istack_destroy(demo)`.

**First sanitizer error:**

```text
==59494==ERROR: AddressSanitizer: heap-use-after-free on address 0x74ff68de0048 at pc 0x626d242b7285 bp 0x7ffc86e46b80 sp 0x7ffc86e46b70
```

**What happened in memory:** The destroy operation released the stack object, but the pointer still held its previous address; `istack_push()` attempted to read fields from that freed object and caused a heap-use-after-free.

**Correction:** Remove the call to `istack_push()` after destruction and rerun `make memcheck`.

### Experiment 3: Omit stack destruction in `balance.c`

**Modification:** Temporarily comment out `istack_destroy(open)` on `balance.c`'s normal successful exit path.

**First sanitizer error:**

```text
==61371==ERROR: LeakSanitizer: detected memory leaks
```

**What happened in memory:** `istack_create()` allocated both a stack structure and its integer array, but exiting without `istack_destroy(open)` left those heap blocks unreleased. The bracket-checking output could still be correct while the program leaked memory.

**Correction:** Restore `istack_destroy(open)` immediately before the final successful `return 0;`, remove any duplicate unreachable return/cleanup code, and rerun `make memcheck`.

## Final verification

Before submitting, verify that all temporary bugs have been removed and the final source code contains exactly one cleanup and successful return on the normal `balance.c` path. Then run:

```sh
make clean
make
make test
make memcheck
make clean
```

Commit the source code, test inputs, expected results, scripts, README, and AI usage disclosure to `hw07/`. Do not commit generated executables.
