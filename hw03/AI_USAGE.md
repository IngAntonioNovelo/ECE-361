ECE 361 — Homework 03: AI Usage
Tool Used
I used OpenAI ChatGPT as a tutor, code reviewer, and debugging assistant while working on HW03.
How I Used AI
- Learning pointers: I asked for step-by-step explanations of addresses (&), dereferencing (*), pointer types, NULL, const, and pointer arithmetic.
- Implementing functions: I wrote attempts at swap_float(), minmax(), find_above(), and reverse(), then asked ChatGPT to review my reasoning and point out errors rather than immediately supply full functions.
- Debugging: I shared compiler and linker errors and used ChatGPT's explanations to understand type mismatches, incorrect loop conditions, missing punctuation, and linking source files in WSL.
- Testing: ChatGPT helped me develop individual smoke tests and provided an example consolidated tests/test_ptr.c test suite, including normal and edge cases.
- Build and documentation: ChatGPT supplied a sample Makefile and assisted with drafts of README.md and this AI usage statement.
An AI Mistake I Identified
When my test initially produced undefined reference to swap_float, ChatGPT suggested that an outdated ptr.o file was the likely cause before verifying what was actually saved and compiled. This diagnosis was premature: the error also occurred when I compiled ptr.c directly. After I saved my source files and recompiled, the linker error disappeared. I learned to check the current directory, saved file contents, compile command, and linker inputs instead of assuming the first explanation is correct.
What I Learned and Verified
Working through the functions helped me distinguish an address from the value stored there, understand why swap_float() receives pointers, walk an array without a[i], and use a return value or pointer to communicate results. I ran GCC commands and examined smoke-test output in WSL. AI-generated examples still need to be read, compiled, tested, and checked against the assignment requirements; AI suggestions are not proof that a program works.
