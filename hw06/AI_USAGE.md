# AI Usage — HW06

I used ChatGPT while working on this assignment to understand C pointers and structures, review error handling and scenario parsing, get help constructing tests, and assemble the replay loop, Makefile, and regression-test examples.

I checked the generated suggestions against the assignment and the actual `iom361_r4.h` API. One suggested version incorrectly passed the `base` pointer to `_iom361_set_sensor1` and `_iom361_set_switches`; I used `grep` to inspect the function prototypes and corrected both calls.

I compiled my modules with GCC warning flags and ran the scenario unit tests. The scenario tests passed (9/9 before the final integration).

Before submission, I will run `make clean && make && make test` and inspect the produced traces.