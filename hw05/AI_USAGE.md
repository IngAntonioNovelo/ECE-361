# AI Usage — ECE 361 Homework 05

**Tool used:** OpenAI ChatGPT

## How I used AI

I used ChatGPT as a programming tutor and coding assistant while developing the combination-lock finite state machine for HW05.

- **Understanding the assignment:** I uploaded the HW05 instructions and Week 5 lecture materials. ChatGPT explained states, events, actions, transition tables, enumerations, structs, pointers, and the difference between `lock_event()` and `lock_step()`.
- **Design and practice:** I worked through the lock's state transitions and practiced tracing input sequences. ChatGPT evaluated my answers and helped me correct mistakes, particularly the behavior of `E_CLOSE` while a combination is being entered.
- **Implementation:** ChatGPT helped me write `lock.h` and `lock.c`. We built the program step by step, I requested a complete review of the 16-entry transition table and the state, event, and action name functions.
- **Debugging:** I provided GCC compiler errors, and ChatGPT helped identify problems such as missing punctuation, a misspelled type name, mismatched function return types, and missing name-array declarations.
- **Testing and build tools:** ChatGPT generated a `Makefile` and a comprehensive `tests/test_lock.c` file covering table entries, event classification, input sequences, and name functions.
- **Program and documentation:** ChatGPT generated `main.c`, a sample `data/codes.txt`, and a `README.md` containing build instructions, a state diagram, and the transition table.

## My involvement

I participated in designing and tracing the state machine, wrote and revised portions of the transition table and functions, ran compiler commands in my WSL environment, and used the compiler output to troubleshoot problems. Some final test and documentation were substantially AI-generated at my request.

I am responsible for reviewing the final code, running `make` and `make test`, and ensuring that the submitted repository behaves as required. This file describes assistance received while working on the homework; it does not imply that AI assistance was used during Shine.
