#include "malloc.h"

#include <unistd.h>

static t_blocks g_blocks = {NULL, NULL, NULL, 0};

size_t get_page_size(void)
{
	static size_t page_size = 0;

	if (page_size == 0)
	{
		page_size = (size_t) sysconf(_SC_PAGESIZE);
	}
	return (page_size);
}

size_t align_on_16(size_t size)
{
	if (size % 16 == 0)
		return (size);
	return ((size / 16 + 1) * 16);
}

size_t align_on_page(size_t size)
{
	size_t page_size;

	page_size = get_page_size();
	if (size % page_size == 0)
	{
		return (size);
	}
	return ((size / page_size + 1) * page_size);
}

size_t get_block_size(enum e_fixed_size fixed_size)
{
	//
	// aligned block metadata + 100 spaces (metadata + aligned data)
	//
	return align_on_page(align_on_16(sizeof(t_block)) + 100 * align_on_16(sizeof(t_space)) + 100 * fixed_size);
}

size_t get_block_size(size_t custom_size)
{
	//
	// block metadata + 
	//
	return align_on_16(sizeof(t_large_block)) + align_on_16(custom_size);
}

void *malloc(size_t size)
{
	return (NULL);
}
