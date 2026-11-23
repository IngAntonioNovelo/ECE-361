# ECE 361 — Homework 8: Binary Search Tree ADT

## Build and test

From `hw08/`:

```sh
make
make test
make memcheck
make clean
```

Public operations: `bst_create`, `bst_destroy`, `bst_insert`, `bst_contains`,
`bst_size`, and `bst_height`. Node and tree structures are private to `bst.c`.
Keys smaller than a node go left; equal or greater keys go right.
Height counts nodes: empty tree 0, a one-node tree 1.

## Original search crash — reproduce, then fix

The initial port kept the original loop condition
`while (current->data != key)`, which dereferences `current` before
checking for NULL. The failing test creates an empty tree and calls
`bst_contains(t, 50, &visits)`.

Captured first sanitizer diagnostic line from the deliberately buggy port:

```text
bst.c:192:15: runtime error: member access within null pointer of type 'const struct node'
```

The buggy test exited with status 1 with UBSan/ASan enabled. The original
sample inserted seven nodes before both searches, so it never exercised
searching an empty tree. The fixed function checks `current != NULL`
before accessing `current->data`; the empty-tree regression test now
passes with zero visits. The initial crash was reproduced with a direct
sanitizer-enabled compile/run; the corrected version is exercised by
`make memcheck`.

## Insertion order and height

The program reads integer keys from standard input until EOF.
It prints either `n N, height H`, `empty tree`, or
`not a number after N values` (with error status for invalid input).

Sample seven-node tree: `n 7, height 3`.

Increasing order (`seq 1 1023 | ./treestats`):

```text
n 1023, height 1023
```

Shuffled order (`seq 1 1023 | sort -R | ./treestats`):

```text
n 1023, height 20
```

Increasing order creates a right-leaning chain, so height equals the number of keys (1023).
Shuffling generally produces a shorter tree, reducing the number of node comparisons needed for searches; the precise height depends on the shuffle.
