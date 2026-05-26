#include "malloc.h"

void *realloc(void *ptr, size_t size)
{
	if (ptr == NULL)
	{
		return malloc(size);
	}
	
	if (size == 0)
	{
		free(ptr);
		return NULL;
	}

	t_space *space = (t_space *)((char *)ptr - align_on_16(sizeof(t_space)));
	size = align_on_16(size);
	if (space->size >= size)
	{
		return ptr;
	}
	return (NULL);
}
