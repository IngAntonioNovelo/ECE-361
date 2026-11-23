#include "bst.h"
#include <stdio.h>

int main(void)
{
	bst_t *t = bst_create();
	if (t == NULL)
	{
		fprintf(stderr, "could not create tree\n");
		return 1;
	}

	int key;
	int result;
	while ((result = scanf(" %d", &key)) == 1)
	{
		if (bst_insert(t, key) != 0)
		{
			fprintf(stderr, "could not insert key\n");
			bst_destroy(t);
			return 1;
		}
	}

	if (result == 0 || ferror(stdin))
	{
		printf("not a number after %zu values\n", bst_size(t));
		bst_destroy(t);
		return 1;
	}

	if (bst_size(t) == 0)
	{
		puts("empty tree");
	}
	else
	{
		printf("n %zu, height %zu\n", bst_size(t), bst_height(t));
	}
	bst_destroy(t);
	return 0;
}
