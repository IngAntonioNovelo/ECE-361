#include "bst.h"
#include <stdio.h>

static int failures = 0;
static void check(const char *label, int ok)
{
	printf("%s: %s\n", ok ? "PASS" : "FAIL", label);
	if (!ok)
	{
		failures++;
	}
}

int main(void)
{
	check("NULL tree rejected", bst_remove(NULL, 10) == -1);
	bst_t *t = bst_create();
	if (t == NULL)
	{
		puts("FAIL: allocate tree");
		return 1;
	}
	check("missing key in empty tree", bst_remove(t, 10) == -1 && bst_size(t) == 0);

	int keys[] = {27, 14, 35, 10, 19, 31, 42};
	int ok = 1;
	for (size_t i = 0; i < sizeof keys / sizeof keys[0]; ++i)
	{
		if (bst_insert(t, keys[i]) != 0)
		{
			ok = 0;
		}
	}
	check("build 7-key tree", ok && bst_size(t) == 7 && bst_height(t) == 3);
	check("remove leaf 10", bst_remove(t, 10) == 0 && bst_size(t) == 6 && !bst_contains(t, 10, NULL));
	check("remove one-child 14", bst_remove(t, 14) == 0 && bst_size(t) == 5 && !bst_contains(t, 14, NULL) && bst_contains(t, 19, NULL));
	size_t visits = 999;
	int removed = bst_remove(t, 27);
	int has31 = bst_contains(t, 31, &visits);
	check("remove two-child root 27 using successor 31", removed == 0 && bst_size(t) == 4 && !bst_contains(t, 27, NULL) && has31 && visits == 1);
	check("missing key leaves tree unchanged", bst_remove(t, 999) == -1 && bst_size(t) == 4);
	bst_destroy(t);

	bst_t *dups = bst_create();
	if (dups == NULL)
	{
		puts("FAIL: allocate duplicate tree");
		return 1;
	}
	check("insert 3 duplicates", bst_insert(dups, 20) == 0 && bst_insert(dups, 20) == 0 && bst_insert(dups, 20) == 0);
	check("remove just one duplicate", bst_remove(dups, 20) == 0 && bst_size(dups) == 2 && bst_contains(dups, 20, NULL));
	check("remove remaining duplicates", bst_remove(dups, 20) == 0 && bst_remove(dups, 20) == 0 && bst_size(dups) == 0);
	check("remove missing duplicate", bst_remove(dups, 20) == -1 && bst_size(dups) == 0);
	bst_destroy(dups);

	bst_t *successor = bst_create();
	if (successor == NULL)
	{
		puts("FAIL: allocate successor tree");
		return 1;
	}
	const int tricky[] = {50, 30, 70, 60, 80, 65};
	for (size_t i = 0; i < sizeof tricky / sizeof tricky[0]; ++i)
	{
		if (bst_insert(successor, tricky[i]) != 0)
		{
			failures++;
		}
	}
	visits = 999;
	check("successor with right child", bst_remove(successor, 50) == 0 && bst_size(successor) == 5 && bst_contains(successor, 60, &visits) && visits == 1 && bst_contains(successor, 65, NULL) && !bst_contains(successor, 50, NULL));
	bst_destroy(successor);

	printf("BONUS: %s (%d failures)\n", failures ? "FAIL" : "PASS", failures);
	return failures ? 1 : 0;
}
