# ECE 361 — Homework 06: Sensor, Panel, and Scenario Replay

## Overview

This program replays a scenario describing temperatures, relative humidity, and switch positions. For every tick, it injects the scenario values into the iom361 simulator, reads the corresponding registers, detects newly pressed switches, toggles the matching LEDs, and prints a UTC trace. Values persist between scenario entries.

## Build

From the `hw06/` directory:

```sh
make
```

The `replay` executable links `main.c`, `sensor.c`, `panel.c`, `scenario.c`, and `iom361_r4.c`. The sensor math library is linked with `-lm`.

## Run

```sh
./replay scenarios/press_hold.txt
./replay scenarios/multiple_switches.txt
```

Example output format:

```text
2026-10-06 08:00:02 temp=21.5 rh=40.0 switches=0x1 pressed=0x0 leds=0x1
```

## Test

```sh
make test
```

`make test` runs panel, sensor, and scenario lookup unit tests in `tests/test_hw06.c`, scenario-file parser tests in `tests/test_scenario.c`, and two regression comparisons through `regress.sh`. The sensor failure test simulates an unsuccessful humidity register read with a test-only linker wrapper (`--wrap=iom361_readReg`); it verifies that `sensor_read()` returns an error without changing any output fields. This wrapper applies only to the unit-test executable, not `replay`.

## Clean

```sh
make clean
```

## Modules

- `sensor.c`: Read temperature and relative humidity registers, mask and convert raw data, and round to tenths.
- `panel.c`: Detect switch press/release edges and update one LED by read-modify-write.
- `scenario.c`: Parse scenario files, reject malformed inputs, and hold the last values between updates.
- `main.c`: Initialize iom361, replay each second, update switch-triggered LEDs, and emit UTC traces.
- `iom361_r4.c`: Instructor-provided simulated hardware implementation.

## Scenarios and expected traces

- `scenarios/press_hold.txt` and `expected/press_hold.txt` test press, hold, release, and press again.
- `scenarios/multiple_switches.txt` and `expected/multiple_switches.txt` test simultaneous presses and a temperature of 91.0 °C.

Expected trace files are compared byte-for-byte using `diff -u`.
