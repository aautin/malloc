#include "malloc.h"

#include "ft_printf.h"

void *malloc(size_t size)
{
	ft_printf("malloc called with size: %zu\n", size);
	(void) size;
	return (NULL);
}
