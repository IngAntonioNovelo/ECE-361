#include "bst.h"
#include <stdio.h>

static int failures = 0;

static void check(const char *name, int ok)
{
	printf("%s: %s\n", ok ? "PASS" : "FAIL", name);
	if (!ok)
	{
		failures++;
	}
}

int main(void)
{
	size_t visits = 999;
	check("contains NULL tree", !bst_contains(NULL, 10, &visits) && visits == 0);
	check("NULL size and height", bst_size(NULL) == 0 && bst_height(NULL) == 0);
	check("insert into NULL tree rejected", bst_insert(NULL, 4) == -1);
	bst_destroy(NULL);
	check("destroy NULL", 1);

	bst_t *empty = bst_create();
	if (empty == NULL)
	{
		puts("FAIL: create empty tree");
		return 1;
	}
	check("create empty tree", bst_size(empty) == 0 && bst_height(empty) == 0);
	visits = 999;
	check("search empty tree regression", !bst_contains(empty, 50, &visits) && visits == 0);
	bst_destroy(empty);

	bst_t *tree = bst_create();
	if (tree == NULL)
	{
		puts("FAIL: create seven-key tree");
		return 1;
	}
	const int keys[] = {27, 14, 35, 10, 19, 31, 42};
	int inserts_ok = 1;
	for (size_t i = 0; i < sizeof keys / sizeof keys[0]; ++i)
	{
		if (bst_insert(tree, keys[i]) != 0)
		{
			inserts_ok = 0;
			break;
		}
	}
	check("insert seven original keys", inserts_ok);
	check("seven-key size 7 height 3", bst_size(tree) == 7 && bst_height(tree) == 3);
	visits = 999;
	check("31 found in 3 visits", bst_contains(tree, 31, &visits) && visits == 3);
	visits = 999;
	check("15 not found in 3 visits", !bst_contains(tree, 15, &visits) && visits == 3);
	check("search with NULL visit pointer", bst_contains(tree, 42, NULL));
	bst_destroy(tree);
	check("destroy populated tree", 1);

	bst_t *duplicates = bst_create();
	if (duplicates == NULL)
	{
		puts("FAIL: create duplicate tree");
		return 1;
	}
	int dup_ok = bst_insert(duplicates, 30) == 0 && bst_insert(duplicates, 30) == 0;
	check("duplicate inserts", dup_ok);
	check("duplicate retained size 2 height 2", bst_size(duplicates) == 2 && bst_height(duplicates) == 2);
	visits = 999;
	check("duplicate search finds first copy", bst_contains(duplicates, 30, &visits) && visits == 1);
	bst_destroy(duplicates);

	bst_t *ascending = bst_create();
	if (ascending == NULL)
	{
		puts("FAIL: create increasing tree");
		return 1;
	}
	int asc_ok = 1;
	for (int i = 1; i <= 12; ++i)
	{
		if (bst_insert(ascending, i) != 0)
		{
			asc_ok = 0;
			break;
		}
	}
	check("increasing inserts", asc_ok);
	check("increasing keys form height equal to size", bst_size(ascending) == 12 && bst_height(ascending) == 12);
	bst_destroy(ascending);

	printf("SUMMARY: %s (%d failures)\n", failures ? "FAIL" : "PASS", failures);
	return failures ? 1 : 0;
}
