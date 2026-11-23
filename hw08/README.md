
## Original BST Search Bug

Test: Searching for key 50 in an empty BST.

The test creates a tree using bst_create() without inserting
any nodes, then calls bst_contains(t, 50, &visits).

The original search loop dereferences current->data before
checking whether current is NULL.

Sanitizer report (first diagnostic line):

bst.c:192:15: runtime error: member access within null pointer of type 'const struct node'

The original example did not expose this bug because it inserted
seven nodes before performing its searches. It never attempted
to search an empty tree.

The corrected implementation checks that current is not NULL
before accessing its data field.
