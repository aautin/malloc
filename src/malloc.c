#include "malloc.h"

#include "libft.h"

#include <unistd.h>
#include <sys/mman.h>

static t_blocks g_blocks = {NULL, NULL, NULL};

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

void* get_free_space(t_block *blocks, size_t aligned_allocation_size)
{
	for (t_block *block = blocks; block != NULL; block = block->next)
	{
		t_space *space = (t_space *)((char *)block + align_on_16(sizeof(t_block)));
		while (true)
		{
			if (!space->taken && space->size >= aligned_allocation_size)
			{
				return space;
			}
			if (space->is_last)
			{
				break;
			}
			space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + space->size);
		}
	}
	return NULL;
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

void prepare_block(t_block *block, size_t block_size, size_t aligned_allocation_size)
{
	t_space* space = (t_space *)((char *)block + align_on_16(sizeof(t_block)));
	
	//
	// If the block data can fit, after the first space, another space (metadata + data), we split the block data into 2 spaces
	// Otherwise, we create only one space with the whole block data
	//
	if (block_size - align_on_16(sizeof(t_block)) >=
		align_on_16(sizeof(t_space)) + aligned_allocation_size + align_on_16(sizeof(t_space)) + 16)
	{
		space->is_last = false;
		
		t_space* next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + aligned_allocation_size);
		next_space->previous = space;
		next_space->size     = block_size - align_on_16(sizeof(t_block)) - align_on_16(sizeof(t_space)) - aligned_allocation_size;
		next_space->taken    = false;
		next_space->is_last  = true;
	}
	else
	{
		space->is_last = true;
		aligned_allocation_size = block_size - align_on_16(sizeof(t_block)) - align_on_16(sizeof(t_space));
	}
	space->previous = NULL;
	space->taken    = true;
	space->size     = aligned_allocation_size;
}

void allocate_space(t_space* space, size_t aligned_allocation_size)
{
	if (space->size >= aligned_allocation_size + align_on_16(sizeof(t_space)) + 16)
	{
		t_space *next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + aligned_allocation_size);

		next_space->previous = space;
		next_space->size     = space->size - aligned_allocation_size - align_on_16(sizeof(t_space));
		next_space->taken    = false;
		next_space->is_last  = space->is_last;
		
		space->is_last       = false;
	}
	else
	{
		aligned_allocation_size = space->size;
	}
	space->taken = true;
	space->size  = aligned_allocation_size;
}

void show_spaces(t_block *blocks)
{
	for (t_block *block = blocks; block != NULL; block = block->next)
	{
		t_space *space = (t_space *)((char *)block + align_on_16(sizeof(t_block)));
		while (true)
		{
			ft_putnbr_fd((int)(space->size), STDOUT_FILENO);
			if (space->taken)
			{
				write(STDOUT_FILENO, " (taken)\n", 9);
			}
			else
			{
				write(STDOUT_FILENO, " (free)\n", 8);
			}
			if (space->is_last)
			{
				break;
			}
			space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + space->size);
		}
	}
}

void show_memory(void)
{
	for (t_block *block = g_blocks.tinies; block != NULL; block = block->next)
	{
		write(STDOUT_FILENO, "Tiny block:\n", 12);
		show_spaces(block);
	}

	for (t_block *block = g_blocks.smalls; block != NULL; block = block->next)
	{
		write(STDOUT_FILENO, "Small block:\n", 13);
		show_spaces(block);
	}

	for (t_block *block = g_blocks.larges; block != NULL; block = block->next)
	{
		write(STDOUT_FILENO, "Large block:\n", 13);
		show_spaces(block);
	}

	write(STDOUT_FILENO, "-------------------------\n\n", 26);
}

void *malloc(size_t size)
{
	t_block **blocks;
	size_t    new_block_size;

	if (size <= TINY)
	{
		blocks = &g_blocks.tinies;
		new_block_size = get_block_size(TINY);
	}
	else if (size <= SMALL)
	{
		blocks = &g_blocks.smalls;
		new_block_size = get_block_size(SMALL);
	}
	else
	{
		blocks = &g_blocks.larges;
		new_block_size = get_large_block_size(size);
	}

	void* free_space = get_free_space(*blocks, align_on_16(size));
	if (free_space != NULL)
	{
		allocate_space(free_space, align_on_16(size));
		
		// show_memory();
		return free_space + align_on_16(sizeof(t_space));
	}
	else
	{
		t_block* new_block = mmap(0, new_block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
		if (new_block == MAP_FAILED)
		{
			return NULL;
		}
		
		prepare_block(new_block, new_block_size, align_on_16(size));
		push_front(blocks, new_block);

		// show_memory();
		return (char *)new_block + align_on_16(sizeof(t_block)) + align_on_16(sizeof(t_space));
	}
}
