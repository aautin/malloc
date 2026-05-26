#include "malloc.h"

int main(void)
{
	void* ptr1 = malloc(42);

	show_memory();

	void* ptr2 = malloc(84);

	show_memory();

	void* ptr1r = realloc(ptr1, 84);

	show_memory();

	free(ptr1r);

	show_memory();

	void* ptr2r = realloc(ptr2, 120);

	show_memory();

	free(ptr2r);

	show_memory();

	return 0;
}