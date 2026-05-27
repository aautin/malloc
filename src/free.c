#include "malloc.h"

/*!
 * @brief Merges the given space with previous one. Use this function only if previous space if not taken.
 */
static void merge_with_previous_space(t_space *space)
{
	space->previous->size    += align_on_16(sizeof(t_space)) + space->size;
	space->previous->is_last  = space->is_last;
}

/*!
 * @brief Merges the given space with next one. Use this function only if next space if not taken.
 */
static void merge_with_next_space(t_space *space)
{
	t_space *next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + space->size);

	space->size    += align_on_16(sizeof(t_space)) + next_space->size;
	space->is_last  = next_space->is_last;
}

void apply(t_space *space)
{
	space->taken = false;

	//
	// Merge with adjacent free spaces
	//
	while (space->previous != NULL && !space->previous->taken)
	{
		merge_with_previous_space(space);
		space = space->previous;
	}
	while (!space->is_last)
	{
		t_space *next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + space->size);
		if (next_space->taken)
		{
			break;
		}
		else
		{
			merge_with_next_space(space);
		}
	}

	//
	// Remove the block if it is empty and not the last one in its list of blocks
	//
	if (space->previous == NULL && space->is_last)
	{
		t_blocks *blocks         = get_blocks();
		t_block  *block_to_unmap = (t_block *)((char *)space - align_on_16(sizeof(t_block)));
		size_t    size_to_unmap;
		t_block** block_list;


		if (space->block_type == TINY_BLOCK && blocks_number(blocks->tinies) > 1)
		{
			size_to_unmap = get_block_size(TINY_SIZE, 0);
			block_list    = &blocks->tinies;
		}
		else if (space->block_type == SMALL_BLOCK && blocks_number(get_blocks()->smalls) > 1)
		{
			size_to_unmap = get_block_size(SMALL_SIZE, 0);
			block_list    = &blocks->smalls;
		}
		else if (space->block_type == LARGE_BLOCK)
		{
			size_to_unmap = align_on_16(sizeof(t_block)) + align_on_16(sizeof(t_space)) + space->size;
			block_list    = &blocks->larges;
		}
		else
		{
			return;
		}

		remove_block_from_list(block_list, block_to_unmap);
		munmap(block_to_unmap, size_to_unmap);
	}
}

void free(void *ptr)
{
	if (ptr == NULL)
	{
		return;
	}

	t_space *space = (t_space *)((char *)ptr - align_on_16(sizeof(t_space)));
	if (!space->taken)
	{
		return;
	}

	pthread_mutex_t* mutex;
	switch (space->block_type)
	{
		case TINY_BLOCK:
			mutex = &get_blocks()->tinies_mutex;
			break;
		case SMALL_BLOCK:
			mutex = &get_blocks()->smalls_mutex;
			break;
		case LARGE_BLOCK:
			mutex = &get_blocks()->larges_mutex;
			break;
	}

	pthread_mutex_lock(mutex);
	apply(space);
	pthread_mutex_unlock(mutex);
}
