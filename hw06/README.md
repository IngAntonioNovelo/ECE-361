# ECE 361 HW06 — Replay

## Build

From `hw06/`:

```sh
make
```

The `replay` executable is built from `main.c`, `sensor.c`, `panel.c`, `scenario.c`, and `iom361_r4.c`.

## Run

```sh
./replay scenarios/press_hold.txt
```

Each output line contains UTC time, temperature, relative humidity, switch state, newly pressed switches, and LED state.

## Test

```sh
make test
```

This builds and runs `tests/test_hw06.c`, `tests/test_scenario.c`, and compares the output of each sample scenario against its checked-in expected trace.

## Clean

```sh
make clean
```

## Module overview

- `scenario.c`: Load and validate the scenario file; find the most recently active values.
- `sensor.c`: Read and convert the simulated sensor registers.
- `panel.c`: Detect pressed/released edges and update LEDs by read-modify-write.
- `main.c`: Inject scenario values into the simulator, poll registers, toggle LEDs on presses, and print UTC traces.

## Scenarios

- `press_hold.txt`: Press, held switch, release, repeated press, and held temperature.
- `multiple_switches.txt`: Multiple simultaneous presses and 91.0 °C sensor test.

