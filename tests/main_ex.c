#include "malloc.h"

int main(void)
{
	void* ptr1 = malloc(42);

	show_alloc_mem_ex();

	((char*) ptr1)[0] = 'a';

	show_alloc_mem_ex();

	void* ptr2 = malloc(84);

	show_alloc_mem_ex();

	void* ptr1r = realloc(ptr1, 84);

	show_alloc_mem_ex();

	free(ptr1r);

	show_alloc_mem_ex();

	void* ptr2r = realloc(ptr2, 120);

	show_alloc_mem_ex();

	free(ptr2r);

	show_alloc_mem_ex();

	return 0;
}