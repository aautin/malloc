#include "malloc.h"

int main(void)
{
	void* ptr1 = malloc(42);

	show_alloc_mem_ex();

	void* ptr2 = malloc(42);

	show_alloc_mem_ex();

	void* ptr3 = malloc(42);

	show_alloc_mem_ex();

	free(ptr1);

	show_alloc_mem_ex();

	free(ptr3);

	show_alloc_mem_ex();

	free(ptr2);

	show_alloc_mem_ex();

	return 0;
}