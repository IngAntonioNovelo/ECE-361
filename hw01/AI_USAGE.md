# AI Usage

I used ChatGPT while completing this homework.

I used it to:

- Review the homework requirements.
- Explain bit masks, shifts, field extraction, and sign extension.
- Help create and debug the initial implementations of the bit-manipulation
  functions.
- Diagnose GCC compiler errors and warnings.
- Help structure `status.h` and `status.c`.
- Help design boundary tests and the Makefile.

One problem with the AI-assisted workflow was the initial test for
`print_binary()`. ChatGPT originally suggested visually comparing the
printed result with an expected value. Because that test was not
automated, the test program reported "0 test(s) failed" even while
`print_binary()` was incorrectly printing `00` instead of `0010 1100`.

I corrected the implementation and replaced the visual check with an
automated test that captures the output of `print_binary()` and compares
it against the expected string.
