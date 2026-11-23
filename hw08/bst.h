
#ifndef BST_H
#define BST_H

#include <stdbool.h>
#include <stddef.h>

/* Opaque binary search tree type. */
typedef struct bst bst_t;

/*
 * Create an empty binary search tree.
 * Takes: No arguments.
 * Returns: Pointer to the new tree, or NULL on allocation failure.
 * Assumes: Caller will eventually destroy the returned tree.
 */
bst_t *bst_create(void);

/*
 * Destroy a binary search tree.
 * Takes: Pointer to a tree, or NULL.
 * Returns: Nothing.
 * Assumes: The tree must not be used after destruction.
 * Passing NULL does nothing.
 */
void bst_destroy(bst_t *t);

/*
 * Insert one integer into the tree.
 * Takes: Tree pointer and integer key.
 * Returns: 0 on success; -1 if t is NULL or allocation fails.
 * Assumes: Smaller values go left; equal or greater values go right.
 * On failure, the tree remains unchanged.
 */
int bst_insert(bst_t *t, int key);

/*
 * Search for an integer in the tree.
 * Takes: Tree pointer, key, and optional visits pointer.
 * Returns: true if found; false otherwise.
 * Assumes: visits may be NULL. Otherwise, *visits receives
 * the number of nodes compared. NULL tree gives 0 visits.
 */
bool bst_contains(const bst_t *t, int key, size_t *visits);

/*
 * Return the number of keys in the tree.
 * Takes: Tree pointer, or NULL.
 * Returns: Number of stored keys, including duplicates.
 * Assumes: NULL tree has size 0.
 */
size_t bst_size(const bst_t *t);

/*
 * Determine the height of the tree.
 * Takes: Tree pointer, or NULL.
 * Returns: Number of nodes along the longest root-to-leaf path.
 * Assumes: Empty or NULL tree has height 0.
 */
size_t bst_height(const bst_t *t);

#endif
