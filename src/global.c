#include "malloc.h"


t_blocks g_blocks = {PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL};

__attribute__((constructor)) void init_global()
{
	pthread_mutex_init(&g_blocks.mutex, NULL);
}

static size_t current_block_size(t_block *block)
{
	t_space *space = (t_space *)((char *)block + align_on_16(sizeof(t_block)));
	size_t size = 0;
	while (true)
	{
		size += space->size;
		if (space->is_last)
		{
			break;
		}
		space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + space->size);
	}
	return size;
}

__attribute__((destructor)) void cleanup_global()
{
    pthread_mutex_destroy(&g_blocks.mutex);
	for (t_block *block = g_blocks.tinies; block != NULL; )
	{
		t_block *next = block->next;
		munmap(block, get_block_size(TINY_BLOCK, 0));
		block = next;
	}

	for (t_block *block = g_blocks.smalls; block != NULL; )
	{
		t_block *next = block->next;
		munmap(block, get_block_size(SMALL_BLOCK, 0));
		block = next;
	}

	for (t_block *block = g_blocks.larges; block != NULL; )
	{
		t_block *next = block->next;
		munmap(block, get_block_size(LARGE_BLOCK, current_block_size(block)));
		block = next;
	}
}

t_blocks* get_blocks()
{
	return &g_blocks;
}
