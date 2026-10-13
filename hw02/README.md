# ECE 361 — Homework 02: Functions, Recursion, and Modules

## Overview

This program reads sensor measurements from standard input. Each valid data line contains a tick, temperature (°C), and humidity (%RH). It reports the number of readings and skipped invalid lines, temperature and humidity minimum/maximum/mean, the longest consecutive run of temperatures **strictly above** a threshold, and a temperature histogram.

HW02 refactors the original single-file `readings.c` program into separate modules. The goal is to improve code organization and reusability **without changing the original output**.

## Files and responsibilities

| File | Responsibility |
| --- | --- |
| `main.c` | Process the optional command-line threshold, own the sensor arrays and variables, call the modules, and print the summary. |
| `reading.h` / `reading.c` | Read lines from a `FILE *`, ignore blank/comment lines, parse valid measurements, count malformed lines, and enforce the array capacity. |
| `stats.h` / `stats.c` | Compute minimum, recursive maximum, mean, and the longest consecutive run above a threshold. |
| `histogram.h` / `histogram.c` | Count readings in 5°C histogram bins and print nonempty bins. |
| `tests/test_stats.c` | Run independent unit tests for the statistics module. |
| `Makefile` | Build the application and tests, record dependencies, and remove generated outputs. |
| `regress.sh`, `data/`, `expected/` | Compare the refactored program's output with the supplied reference output. |

## Module diagram

Arrows indicate function calls between modules:

```text
                +------------------+
                |      main.c      |
                | CLI and summary  |
                +------------------+
                    |      |      |
          +---------+      |      +----------+
          v                v                 v
 +----------------+ +----------------+ +------------------+
 |   reading.c    | |    stats.c     | |   histogram.c    |
 | Parse readings | | Min/max/mean,  | | Histogram output |
 |                | | longest run    | |                  |
 +----------------+ +----------------+ +------------------+
```

Each module provides public declarations through its corresponding `.h` header. The source files do not depend on global sensor arrays; `main.c` owns the arrays and passes them to functions. The maximum function is recursive and works with either the temperature or humidity array.

## Building and running

From the `hw02/` directory, with GCC and `make` installed:

```bash
make
./readings < data/normal.txt
./readings 25 < data/edge.txt
./readings < data/empty.txt
```

The optional command-line argument is a temperature threshold in °C. If omitted, it defaults to `30.0`.

The project uses `gcc -std=c11 -Wall -Wextra -g`. `make` builds one `.o` file per `.c` module and links them into `readings`. Dependencies on included headers are declared in the Makefile so only affected objects need recompilation.

## Testing

Run all tests:

```bash
make test
```

This builds and runs `tests/test_stats`, then executes `sh regress.sh`. The unit tests cover a one-value array, maximum at the beginning and end, a value equal to the threshold ending a run, no run above the threshold, and a recursive maximum on 10,000 values.

The regression script compares results against `expected/` for `normal.txt`, `edge.txt`, `empty.txt`, and `edge.txt` with threshold 25. `diff` must find no differences, including whitespace.

**Most recent local test results (reported after correcting the `test_stats.c` filename):** 7 unit tests passed, 0 failed; all 4 regression tests passed.

To remove generated `.o` files and executables:

```bash
make clean
```

## Assumptions and boundaries

- The reading arrays have capacity `MAX_READINGS` (1000). Extra data beyond capacity is not processed.
- Lines beginning with `#` after optional spaces/tabs and blank lines are ignored; lines that cannot provide the three required values count as skipped.
- `stats_min`, `stats_max`, and `stats_mean` require `n >= 1`. `main.c` handles the no-readings case before calling them.
- `stats_longest_run_above` uses a strict `>` comparison: a reading equal to the threshold ends a run. If no run exists, the starting index is `-1`.
- Histogram bins are 5°C wide, and values outside the displayed 0–100°C interval are assigned to the nearest endpoint bin, matching the starter program.
