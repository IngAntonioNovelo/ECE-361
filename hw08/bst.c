#include "bst.h"
#include <stdlib.h>

/* Based on struct node in the original BinarySearchTree.c. */
struct node {
	int data;
	struct node *leftChild;
	struct node *rightChild;
};

/* Private tree state; no global tree pointer. */
struct bst {
	struct node *root;
	size_t size;
};

/* Creates the private tree descriptor (original program used a global root). */
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

/* Port of insert() in BinarySearchTree.c; duplicate keys follow rightChild. */
int bst_insert(bst_t *t, int key)
{
	if (t == NULL)
	{
		return -1;
	}

	/* Allocate first: on failure the tree remains unchanged. */
	struct node *tempNode = malloc(sizeof *tempNode);
	if (tempNode == NULL)
	{
		return -1;
	}
	tempNode->data = key;
	tempNode->leftChild = NULL;
	tempNode->rightChild = NULL;

	if (t->root == NULL)
	{
		t->root = tempNode;
		t->size++;
		return 0;
	}

	struct node *parent = NULL;
	struct node *current = t->root;
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

	if (key < parent->data)
	{
		parent->leftChild = tempNode;
	}
	else
	{
		parent->rightChild = tempNode;
	}
	t->size++;
	return 0;
}

/* Postorder destructor; the original BinarySearchTree.c had no cleanup. */
static void destroy_nodes(struct node *node)
{
	if (node == NULL)
	{
		return;
	}
	destroy_nodes(node->leftChild);
	destroy_nodes(node->rightChild);
	free(node);
}

void bst_destroy(bst_t *t)
{
	if (t == NULL)
	{
		return;
	}
	destroy_nodes(t->root);
	free(t);
}

/* Port of search() in BinarySearchTree.c, without printing or global root.
 * Original crash: while (current->data != data) dereferenced NULL on an
 * empty tree. This corrected version checks current before dereferencing.
 */
bool bst_contains(const bst_t *t, int key, size_t *visits)
{
	if (visits != NULL)
	{
		*visits = 0;
	}
	if (t == NULL)
	{
		return false;
	}

	const struct node *current = t->root;
	while (current != NULL)
	{
		if (visits != NULL)
		{
			(*visits)++;
		}
		if (current->data == key)
		{
			return true;
		}
		if (key < current->data)
		{
			current = current->leftChild;
		}
		else
		{
			current = current->rightChild;
		}
	}
	return false;
}

size_t bst_size(const bst_t *t)
{
	return t == NULL ? 0 : t->size;
}

/* Recursive longest-path computation; nodes, not edges, are counted. */
static size_t height_nodes(const struct node *node)
{
	if (node == NULL)
	{
		return 0;
	}
	size_t left = height_nodes(node->leftChild);
	size_t right = height_nodes(node->rightChild);
	return 1 + (left > right ? left : right);
}

size_t bst_height(const bst_t *t)
{
	if (t == NULL)
	{
		return 0;
	}
	return height_nodes(t->root);
}

/* Bonus: remove the first matching key found during BST search.
 * A two-child node is replaced with the smallest key in its right subtree.
 * Based on the original search and the BST ordering rule.
 */
int bst_remove(bst_t *t, int key)
{
	if (t == NULL)
	{
		return -1;
	}

	/* link points at whichever pointer currently owns the node. */
	struct node **link = &t->root;
	while (*link != NULL && (*link)->data != key)
	{
		if (key < (*link)->data)
		{
			link = &(*link)->leftChild;
		}
		else
		{
			link = &(*link)->rightChild;
		}
	}

	if (*link == NULL)
	{
		return -1;
	}

	struct node *target = *link;
	if (target->leftChild == NULL)
	{
		/* Zero or one child: splice in its right subtree. */
		*link = target->rightChild;
		free(target);
	}
	else if (target->rightChild == NULL)
	{
		/* Exactly one child: splice in its left subtree. */
		*link = target->leftChild;
		free(target);
	}
	else
	{
		/* Two children: find the inorder successor. */
		struct node **successor_link = &target->rightChild;
		while ((*successor_link)->leftChild != NULL)
		{
			successor_link = &(*successor_link)->leftChild;
		}

		struct node *successor = *successor_link;
		target->data = successor->data;
		*successor_link = successor->rightChild;
		free(successor);
	}

	t->size--;
	return 0;
}
