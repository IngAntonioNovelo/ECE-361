# ECE 361 — HW07 AI Usage Disclosure

## Tool used

**ChatGPT (OpenAI)** was used as a learning, code-development, debugging, testing, and documentation assistant for Homework 7. AI use was permitted for the assignment. The interaction took place while preparing the homework; the separate Shine assessment must be completed without AI or other supporting materials.

## How AI assisted

### Understanding and planning

- Explained dynamic memory, heap allocation, pointer dereferencing, ownership, dangling pointers, LIFO stacks, and capacity versus count.
- Helped turn the assignment into pseudocode for the `istack` ADT and the `balance` bracket checker.
- Provided comprehension questions on pointers, `malloc()`, `realloc()`, `free()`, stack operations, errors, and testing; reviewed my written explanations and corrected misunderstandings.

### Code and implementation

- Suggested and supplied C code examples, including complete or partial implementations for stack creation, destruction, push, pop, peek, metadata accessors, and copying the stack to a new array.
- Explained why `realloc()` must use a temporary pointer to preserve the original allocation when resizing fails, and how to check allocation sizes for overflow.
- Supplied starter and completed examples of `balance.c`, including bracket matching, location tracking, error reporting, end-of-input handling, and cleanup.
- Helped design, extend, and review `tests/test_istack.c`, including empty-stack behavior, LIFO operations, 8-to-16 growth, 1,000 pushes/pops, independent stacks, and copied-array ownership.
- Suggested the regression inputs, expected output/status files, a shell regression runner, and a Makefile with `make`, `make test`, `make memcheck`, and `make clean` targets.

### Debugging and memory checks

- Helped interpret build and test output that I ran locally in WSL Ubuntu.
- Walked through three intentionally introduced memory bugs: destroying a stack twice, pushing after destruction, and omitting stack destruction from `balance.c`.
- Helped interpret the AddressSanitizer `heap-use-after-free` errors and LeakSanitizer memory-leak report, including why an earlier leak experiment failed to trigger because the code returned before reaching the modified section.
- Assisted with Git status, divergent local/remote branch history, pushing the assignment, and distinguishing source files from generated executables.

### Documentation

- Helped draft `README.md`, including project overview, build and test instructions, test results, required memory-error experiments, and explanation of the sanitizer findings.
- Helped draft this `AI_USAGE.md` disclosure.

## My work and verification

I worked through the explanations, answered comprehension checks, added and modified files in the repository, ran compiler and test commands in WSL Ubuntu, and shared actual terminal output for review. The AI-provided examples and suggestions were used as part of the implementation process, rather than only as general conceptual advice.

The following local results were reported after the correct source was restored from the first two intentional bugs and in a clean sanitizer run before Experiment 3:

```text
Summary: 27 passed, 0 failed
Regression summary: 6 passed, 0 failed
```

Both programs compiled using `gcc -std=c11 -Wall -Wextra -Werror -g` without warnings in the reported tests. `make memcheck` used AddressSanitizer and UndefinedBehaviorSanitizer. The intentional memory-error experiments produced actual sanitizer reports, which are recorded in `README.md`.

Before final submission, the intentionally leaking `balance.c` must be restored and the clean test/memory-check sequence rerun. Final responsibility for checking, understanding, and submitting the code is mine.
