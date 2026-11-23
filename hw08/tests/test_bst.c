
#include "bst.h"
#include <stdio.h>

int main(void)
{
	/* Create an empty binary search tree. */
	bst_t *t = bst_create();

	if (t == NULL)
	{
		printf("FAIL: Could not create BST\n");
		return 1;
	}

	/* Attempt to search an empty tree. */
	size_t visits = 0;

	printf("TEST: Searching empty BST\n");
	fflush(stdout);

	bool found = bst_contains(t, 50, &visits);

	
    /* Verify that searching an empty tree is safe. */
    if (found != false || visits != 0)
    {
        printf("FAIL: Search empty BST\n");
        bst_destroy(t);
        return 1;
    }

    
    printf("PASS: Search empty BST\n");

    /* Free the BST structure before exiting. */
    bst_destroy(t);

    return 0;
	
}
