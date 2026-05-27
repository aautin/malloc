#include "malloc.h"
#include "libft.h"

static void* get_free_space(t_block *blocks, size_t aligned_allocation_size)
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

static void prepare_block(t_block *block, size_t block_size, size_t aligned_allocation_size, t_block_type block_type)
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
		next_space->previous   = space;
		next_space->size       = block_size - align_on_16(sizeof(t_block)) - align_on_16(sizeof(t_space)) - aligned_allocation_size;
		next_space->taken      = false;
		next_space->is_last    = true;
		next_space->block_type = space->block_type;
	}
	else
	{
		space->is_last = true;
		aligned_allocation_size = block_size - align_on_16(sizeof(t_block)) - align_on_16(sizeof(t_space));
	}
	space->previous   = NULL;
	space->taken      = true;
	space->size       = aligned_allocation_size;
	space->block_type = block_type;
}

static void allocate_space(t_space* space, size_t aligned_allocation_size, t_block_type block_type)
{
	if (space->size >= aligned_allocation_size + align_on_16(sizeof(t_space)) + 16)
	{
		t_space *new_next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + aligned_allocation_size);
		
		if (!space->is_last)
		{
			t_space *next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + space->size);

			if (!next_space->taken)
			{
				new_next_space->previous = space;
				new_next_space->size     = space->size - aligned_allocation_size - align_on_16(sizeof(t_space)) + align_on_16(sizeof(t_space)) + next_space->size;
				new_next_space->taken    = false;
				new_next_space->is_last  = next_space->is_last;
				new_next_space->block_type = space->block_type;
				return;
			}
		}
		
		new_next_space->previous = space;
		new_next_space->size     = space->size - aligned_allocation_size - align_on_16(sizeof(t_space));
		new_next_space->taken    = false;
		new_next_space->is_last  = space->is_last;
		new_next_space->block_type = space->block_type;

		space->is_last = false;
	}
	else
	{
		aligned_allocation_size = space->size;
	}
	space->block_type = block_type;
	space->taken = true;
	space->size  = aligned_allocation_size;
}

static void* apply(t_block** blocks, size_t size, t_block_type type)
{
	size_t new_block_size = get_block_size(type, size);

	void* free_space = get_free_space(*blocks, align_on_16(size));
	if (free_space != NULL)
	{
		allocate_space(free_space, align_on_16(size), type);
		return free_space + align_on_16(sizeof(t_space));
	}
	else
	{
		t_block* new_block = mmap(0, new_block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
		if (new_block == MAP_FAILED)
		{
			return NULL;
		}

		prepare_block(new_block, new_block_size, align_on_16(size), type);
		push_front(blocks, new_block);

		return (char *)new_block + align_on_16(sizeof(t_block)) + align_on_16(sizeof(t_space));
	}
}

void *malloc(size_t size)
{
	t_block**        blocks;
	pthread_mutex_t* mutex;

	t_block_type type = get_type(size);
	switch (type)
	{
		case TINY_BLOCK:
			blocks = &get_blocks()->tinies;
			mutex = &get_blocks()->tinies_mutex;
			break;
		case SMALL_BLOCK:
			blocks = &get_blocks()->smalls;
			mutex = &get_blocks()->smalls_mutex;
			break;
		case LARGE_BLOCK:
			blocks = &get_blocks()->larges;
			mutex = &get_blocks()->larges_mutex;
			break;
	}

	pthread_mutex_lock(mutex);
	void* ptr = apply(blocks, size, type);
	pthread_mutex_unlock(mutex);
	return ptr;
}
