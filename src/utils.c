#include "malloc.h"

size_t get_page_size(void)
{
	static size_t page_size = 0;

	if (page_size == 0)
	{
		page_size = (size_t) sysconf(_SC_PAGESIZE);
	}
	return page_size;
}

size_t align_on_16(size_t size)
{
	if (size % 16 == 0)
		return (size);
	return (size / 16 + 1) * 16;
}

size_t align_on_page(size_t size)
{
	size_t page_size;

	page_size = get_page_size();
	if (size % page_size == 0)
	{
		return size;
	}
	return (size / page_size + 1) * page_size;
}

size_t get_block_size(enum e_fixed_size fixed_size)
{
	return align_on_page(align_on_16(sizeof(t_block)) + 100 * align_on_16(sizeof(t_space)) + 100 * fixed_size);
}

size_t get_large_block_size(size_t custom_size)
{
	return align_on_16(sizeof(t_block)) + align_on_16(sizeof(t_space)) + align_on_16(custom_size);
}

void push_front(t_block **blocks, t_block *new_block)
{
	new_block->next = *blocks;
	new_block->previous = NULL;
	if (*blocks != NULL)
	{
		(*blocks)->previous = new_block;
	}
	*blocks = new_block;
}

bool is_in_blocks(t_block *blocks, t_block *to_find)
{
	for (t_block *block = blocks; block != NULL; block = block->next)
	{
		if (block == to_find)
		{
			return true;
		}
	}
	return false;
}

size_t blocks_number(t_block *blocks)
{
	size_t number = 0;
	for (t_block *block = blocks; block != NULL; block = block->next)
	{
		number += number;
	}
	return number;
}

void remove_block_from_list(t_block **blocks, t_block *to_remove)
{
	if (*blocks == to_remove)
	{
		*blocks = to_remove->next;
	}
	else
	{
		to_remove->previous->next = to_remove->next;
	}

	if (to_remove->next != NULL)
	{
		to_remove->next->previous = to_remove->previous;
	}
}
