
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

/*
 * Insert a key into the binary search tree.
 * Adapted from the original BinarySearchTree.c insertion function.
 */
int bst_insert(bst_t *t, int key)
{
	/* Step 1: Validate the tree pointer. */
	if (t == NULL)
	{
		return -1;
	}

	/* Step 2: Allocate a new node. */
	struct node *tempNode = malloc(sizeof *tempNode);

	if (tempNode == NULL)
	{
		return -1;
	}

	/* Step 3: Initialize the new node. */
	tempNode->data = key;
	tempNode->leftChild = NULL;
	tempNode->rightChild = NULL;

	/* Step 4: Handle an empty tree. */
	if (t->root == NULL)
	{
		t->root = tempNode;
		t->size++;
		return 0;
	}

	/* Step 5: Initialize traversal pointers. */
	struct node *parent = NULL;
	struct node *current = t->root;

	/* Step 6: Find the insertion location. */
	while (current != NULL)
	{
		parent = current;

		if (key < current->data)
		{
			current = current->leftChild;
		}
		else
		{
			current = current->rightChild;
		}
	}

	/* Step 7: Attach the new node to its parent. */
	if (key < parent->data)
	{
		parent->leftChild = tempNode;
	}
	else
	{
		parent->rightChild = tempNode;
	}

	/* Step 8: Update the number of nodes. */
	t->size++;

	/* Step 9: Report successful insertion. */
	return 0;

}

/*
 * Return the number of nodes in the tree.
 * Return 0 if the tree pointer is NULL.
 */
size_t bst_size(const bst_t *t)
{
	if (t == NULL)
	{
		return 0;
	}

	return t->size;
}

/*
 * Recursively free the nodes in a subtree.
 * Uses postorder traversal.
 */
static void destroy_nodes(struct node *node)
{
	/* Base case: there is no node to free. */
	if (node == NULL)
	{
		return;
	}

	/* Visit the left subtree before its parent; NULL children hit the base case. */
	destroy_nodes(node->leftChild);

	/* Visit the right subtree after the left and before freeing this node. */
	destroy_nodes(node->rightChild);

	/* Both subtrees are freed, so release their parent last. */
	free(node);
}

/*
 * Destroy a BST and release its allocated memory.
 * Accepts NULL without performing any operation.
 */
void bst_destroy(bst_t *t)
{
	/* Step 1: Check whether the tree exists. */
	if (t == NULL)
	{
		return;
	}

	/* Step 2: Free all nodes using postorder traversal. */
	destroy_nodes(t->root);

	/* Step 3: Free the tree structure after its nodes. */
	free(t);
}

/*
 * Search for a key in the BST.
 * Adapted from search() in the original BinarySearchTree.c.
 */
bool bst_contains(const bst_t *t, int key, size_t *visits)
{
	/* Initialize the optional visit counter. */
	if (visits != NULL)
	{
		*visits = 0;
	}

	/* A NULL tree cannot contain any keys. */
	if (t == NULL)
	{
		return false;
	}

	/* Begin searching at the root. */
	const struct node *current = t->root;

	/* Follow the only subtree that can contain the key. */
	while (current != NULL)
	{
		/* Count each node compared, including a matching node. */
		if (visits != NULL)
		{
			(*visits)++;
		}

		if (key == current->data)
		{
			return true;
		}

		/* Smaller keys go left; larger keys go right. */
		if (key < current->data)
		{
			current = current->leftChild;
		}
		else
		{
			current = current->rightChild;
		}
	}

	/* Reaching a missing child means the key is absent. */
	return false;
}
