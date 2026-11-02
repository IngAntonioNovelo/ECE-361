# ECE 361 — Homework 05: Combination Lock State Machine

## Overview

This project implements a finite state machine (FSM) for a combination lock in C. The combination is **3-1-4**. A correct digit advances the lock through `LOCKED`, `GOT1`, and `GOT2`; the final correct digit moves to `OPEN` and performs `A_UNLOCK`.

A wrong digit resets the sequence to `LOCKED` and performs `A_BEEP`. A timeout while entering the code resets to `LOCKED` silently. Closing an already-locked door does not change the state, even partway through the combination. While the door is open, digit inputs have no effect; closing the door or timing out returns the machine to `LOCKED` with `A_LOCK`.

The FSM uses a **constant 4 × 4 transition table**. Each cell stores the *next state* and *action*. `lock_step()` reads one cell rather than choosing behavior through a chain of `if` or `switch` statements.

## Files

| File | Purpose |
|---|---|
| `lock.h` | Enumerations, `transition_t`, table declaration, and function prototypes |
| `lock.c` | All 16 transitions; event classification; step and name functions |
| `main.c` | Reads input characters and prints each recognized transition |
| `tests/test_lock.c` | Tests transition cells, event classification, sequences, and names |
| `data/codes.txt` | Sample input for running the lock |
| `Makefile` | Build, test, and clean targets |
| `README.md` | Build instructions, state diagram, transition table, and design explanation |
| `AI_USAGE.md` | Separate disclosure of how AI assistance was used |

## Build and run

Run these commands from the **`hw05/` directory** in a Linux terminal (or WSL Ubuntu):

```bash
make
printf '314c\n' | ./lock
make test
make clean
```

To run input from the sample file:

```bash
make
./lock < data/codes.txt
```

- `make` compiles `main.c` and `lock.c` into an executable named `lock`.
- `make test` builds and runs `tests/test_lock`, printing `PASS` or `FAIL` for each test and a final summary; test failures cause a nonzero exit status.
- `make clean` removes object files and compiled executables.

### Example output

Input: `314c`

```text
LOCKED 3 RIGHT -> GOT1 NONE
GOT1 1 RIGHT -> GOT2 NONE
GOT2 4 RIGHT -> OPEN UNLOCK
OPEN c CLOSE -> LOCKED LOCK
final: LOCKED
```

The program reads standard input **one character at a time**. Digits, `c` (close), and `t` (timeout) produce events; other characters, including spaces and newlines, are ignored. A newline **does not reset** the lock. The current state persists until a table transition changes it or input ends.

The assignment simulates timer expiration using the input character `t`; `main.c` does not measure five seconds in real time.

## State diagram (ASCII)

Successful path through the combination:

```text
                    RIGHT/NONE           RIGHT/NONE         RIGHT/UNLOCK
    [LOCKED] ----------------> [GOT1] ----------------> [GOT2] ----------------> [OPEN]
```

Other transitions, which complete the diagram:

```text
From LOCKED:
    WRONG/BEEP       -> LOCKED (self-loop)
    CLOSE/NONE       -> LOCKED (self-loop)
    TIMEOUT/NONE     -> LOCKED (self-loop)

From GOT1:
    WRONG/BEEP       -> LOCKED
    CLOSE/NONE       -> GOT1   (self-loop)
    TIMEOUT/NONE     -> LOCKED

From GOT2:
    WRONG/BEEP       -> LOCKED
    CLOSE/NONE       -> GOT2   (self-loop)
    TIMEOUT/NONE     -> LOCKED

From OPEN:
    RIGHT/NONE       -> OPEN   (self-loop)
    WRONG/NONE       -> OPEN   (self-loop)
    CLOSE/LOCK       -> LOCKED
    TIMEOUT/LOCK     -> LOCKED
```

The arrow labels are `EVENT/ACTION`. `LOCKED` is the initial state. Every table entry is represented either in the successful path or in the additional arrows above.

## Complete transition table

Each cell shows **next state / action**. `RIGHT`, `WRONG`, `CLOSE`, and `TIMEOUT` in the headings mean the corresponding `E_` event values; action names omit their `A_` prefix for readability.

| Current state | E_RIGHT | E_WRONG | E_CLOSE | E_TIMEOUT |
|---|---|---|---|---|
| **LOCKED** | GOT1 / NONE | LOCKED / BEEP | LOCKED / NONE | LOCKED / NONE |
| **GOT1** | GOT2 / NONE | LOCKED / BEEP | GOT1 / NONE | LOCKED / NONE |
| **GOT2** | OPEN / UNLOCK | LOCKED / BEEP | GOT2 / NONE | LOCKED / NONE |
| **OPEN** | OPEN / NONE | OPEN / NONE | LOCKED / LOCK | LOCKED / LOCK |

The `E_RIGHT` column for `OPEN` is explicitly defined even though `lock_event(OPEN, digit)` classifies digits as `E_WRONG`. Both digit-event transitions in `OPEN` are harmless.

## How the code works

1. `main()` starts with `current_state = LOCKED` and reads a character using `getchar()`.
2. `lock_event(current_state, character)` converts the character into `E_RIGHT`, `E_WRONG`, `E_CLOSE`, or `E_TIMEOUT`. It returns `-1` for an unrecognized character.
3. Valid events are passed to `lock_step(current_state, event, &action)`. The function performs **one table lookup**, writes the action through the pointer, and returns the next state.
4. `main()` prints the previous state, character, event, next state, and action, and then saves the next state as the current state.
5. At end-of-file, the program prints `final: STATE`.

`state_name()`, `event_name()`, and `action_name()` convert enum values into printable text using constant name arrays. They return `"?"` for invalid enum values.

## Tests

`tests/test_lock.c` checks the 16 transition-table entries and their `lock_step()` results, correct and incorrect digit handling (including input while open), close and timeout events, invalid characters, readable names, and full input sequences. Required sequences include:

| Input | Expected final state | Explanation |
|---|---|---|
| `314` | `OPEN` | Correct complete combination |
| `3314` | `LOCKED` | Second `3` is wrong and resets progress |
| `31t4` | `LOCKED` | Timeout resets progress before `4` |
| `314c` | `LOCKED` | Closing an open door locks it again |

The test executable returns zero when all tests pass, and a nonzero value if any test fails.

## AI usage

See `AI_USAGE.md` for the project-specific disclosure of AI assistance. Review that file to ensure it accurately describes your own work and verification.
