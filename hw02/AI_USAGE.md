# ECE 361 — HW02 AI Usage

## Tool used

I used **ChatGPT (OpenAI)** while working on HW02.

## How I used it

- Asked for a walkthrough of the HW02 starter program and the assignment requirements.
- Used ChatGPT to help refactor the original single-file program into the `reading`, `stats`, `histogram`, and `main` modules, including header interfaces and the recursive maximum.
- Used ChatGPT to help create a Makefile, statistics unit tests, and these documentation drafts.
- Asked questions about compiler/build errors and checked results using GCC and `make` in WSL Ubuntu.

## A problem I found and fixed

When I first ran `make test`, it reported:

```text
make: *** No rule to make target 'tests/test_stats.c', needed by 'tests/test_stats'.  Stop.
```

The test source file in my directory did not have the `.c` extension, so its filename did not match the Makefile dependency. I corrected the filename to `tests/test_stats.c` and ran the tests again.

I also investigated an earlier function-declaration error involving `stats_longest_run_above`. Comparing `stats.h` and `stats.c` showed that the uploaded versions matched, so I checked the names and local files rather than assuming the function implementation was missing.

## How I verified the result

In WSL Ubuntu, I ran:

```bash
make test
make clean
```

`make test` reported **7 passed, 0 failed** for unit tests, followed by **4 PASS** regression results. `make clean` removed the generated object files and executables.

AI helped produce and explain code, but successful compilation and the tests in my local environment were used to verify its behavior. I am responsible for understanding and explaining the code for the Shine assessment.
