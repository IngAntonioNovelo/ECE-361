# AI Usage — ECE 361 HW06

I used ChatGPT as a programming tutor while implementing HW06. I worked through C structures, pointers, register reads, switch-edge detection, scenario parsing, and the replay loop with explanations and code reviews. ChatGPT also helped create or revise tests, the integration code in `main.c`, the Makefile, the regression scenarios and expected traces, and the documentation.

One AI-generated suggestion was incorrect: it passed the `base` pointer to `_iom361_set_sensor1()` and `_iom361_set_switches()`. I checked `iom361_r4.h` with `grep`, found that these functions take only the sensor values or switch value, and corrected both calls.

Another suggested test was initially placed in `main.c` rather than `tests/test_hw06.c`, causing a compilation error because `check()` belongs to the test program. I moved the test into the correct file and rebuilt.

I verified the result with GCC using `-std=c11 -Wall -Wextra -Werror` and with `make clean`, `make`, and `make test`. The tests include a test-only injected humidity-register failure to verify that an unsuccessful `sensor_read()` leaves the output unchanged.

The final test status should be confirmed against the terminal output before submission. I am responsible for checking the behavior and being able to explain the code during Shine.
