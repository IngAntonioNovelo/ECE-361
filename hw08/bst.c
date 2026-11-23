
#include "bst.h"
#include <stdlib.h>

/*
 * One node of the binary search tree.
 * Based on struct node in the original BinarySearchTree.c.
 */
struct node {
	int data;
	struct node *leftChild;
	struct node *rightChild;
};

/*
 * Private binary search tree structure.
 * root points to the first node, or NULL when empty.
 * size tracks the number of stored keys.
 */
struct bst {
	struct node *root;
	size_t size;
};

/*
 * Create an empty binary search tree.
 * Returns a pointer to the tree, or NULL if allocation fails.
 */
bst_t *bst_create(void)
{
	bst_t *t = malloc(sizeof *t);

	if (t == NULL)
	{
		return NULL;
	}

	t->root = NULL;
	t->size = 0;

	return t;
}
