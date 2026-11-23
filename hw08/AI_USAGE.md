# AI usage — ECE 361 HW08

I used ChatGPT as a step-by-step tutor to understand BST insertion, pointers,
recursion, memory ownership, and sanitizer error reports; to review C code; and
to help prepare regression tests, build rules, and documentation. I reviewed
and ran the generated code with GCC and sanitizers locally.

One issue the model missed: it initially assumed I had kept the original
unsafe search loop, but inspection of `bst.c` showed that my implementation
already had `while (current != NULL)`. We found the difference by printing
`bst_contains()` with `grep` and then reproduced the original empty-tree
NULL-dereference by temporarily restoring the buggy condition. We restored
the safe version after documenting the failure. A second issue caught by
LeakSanitizer was an early test version that did not free the empty BST;
adding `bst_destroy(t)` on the success path resolved it.

The code and results are my responsibility, and I used the build and test
commands to check rather than relying only on AI explanations.
