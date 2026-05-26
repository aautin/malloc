#include "malloc.h"

int main(void)
{
	void* ptr1 = malloc(42);

	show_alloc_mem();

	void* ptr2 = malloc(84);

	show_alloc_mem();

	void* ptr1r = realloc(ptr1, 84);

	show_alloc_mem();

	free(ptr1r);

	show_alloc_mem();

	void* ptr2r = realloc(ptr2, 120);

	show_alloc_mem();

	free(ptr2r);

	show_alloc_mem();

	return 0;
}