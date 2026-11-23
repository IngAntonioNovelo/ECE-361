# ECE 361 HW08 — Optional Bonus: BST Removal

The original seven-key tree, insertion order `27 14 35 10 19 31 42`:

```text
         27
        /  \
      14    35
     /  \  /  \
   10   19 31  42
```

## 1. Remove 10 — leaf (zero children)

```text
         27
        /  \
      14    35
        \  /  \
        19 31  42
```

Node 10 has no children, so its parent `14`'s `leftChild` becomes NULL and node 10 is freed. Size changes from 7 to 6.

## 2. Remove 14 — one child

```text
         27
        /  \
      19    35
           /  \
          31  42
```

Node 14 has a single child (19). Its parent's left pointer is redirected to 19, then 14 is freed. Size changes from 6 to 5.

## 3. Remove 27 — two children

```text
         31
        /  \
      19    35
              \
              42
```

Node 27 has two children. Its inorder successor is the smallest key in its right subtree: 31. Copy key 31 into the root, unlink the old successor node from `35->leftChild`, and free that old successor. Exactly one node is freed. Size changes from 5 to 4.

## Implementation

`bst_remove(t, key)` follows the tree's search rule and removes the **first matching node encountered**, retaining additional copies of duplicate keys. A NULL tree or absent key returns -1 without changing the tree. The implementation uses a pointer-to-pointer for the parent's selected child (or root), which simplifies safely replacing the removed node. All cases free exactly one node on success.

## Testing

Run `make bonus`. The target builds `tests/test_remove.c` with AddressSanitizer and UndefinedBehaviorSanitizer, runs it with leak detection, and reports PASS or FAIL with a nonzero exit status on failure. Tests cover the three specified removals, missing key, NULL tree, duplicates, and a successor that has a right child.
