# ECE 361 Homework 1

This homework implements a small C library for bit manipulation and
decoding a 16-bit thermostat status word.

## Bit-Manipulation Library

The library provides four functions:

- `print_binary()` prints the lowest requested number of bits in binary.
- `get_field()` extracts a bit field from a 32-bit word.
- `set_field()` replaces a bit field in a 32-bit word.
- `sign_extend()` interprets a field as a two's-complement signed value.

Valid widths are 1 through 32.

For `get_field()` and `set_field()`, valid positions are 0 through 31,
and `pos + width` must not exceed 32.

`get_field()` returns 0 for an invalid field specification.

`set_field()` returns the original word unchanged for an invalid field
specification.

If the value passed to `set_field()` is wider than the destination
field, only the lowest `width` bits are used.

`print_binary()` returns without printing anything when width is outside
the range 1 through 32.

`sign_extend()` supports widths from 1 through 32. It returns 0 when
given an invalid width.

## Thermostat Status Word

`status_unpack()` decodes a 16-bit thermostat status word into a
`status_t` structure.

The fields are:

- Bit 0: HEAT
- Bit 1: COOL
- Bit 2: FAN
- Bit 3: FAULT
- Bits 4-6: MODE
- Bit 7: reserved
- Bits 8-15: signed 8-bit SETPOINT

MODE values are:

- 0: OFF
- 1: HEAT
- 2: COOL
- 3: AUTO
- 4: FAN_ONLY
- 5-7: invalid

For invalid MODE values, `status_unpack()` preserves the raw three-bit
value in `status_t.mode`. Therefore values 5, 6, and 7 can be detected
and reported by the caller rather than being replaced with another
value.

## Building

Compile all source files into object files with:

```sh
make
