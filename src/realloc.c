#include "malloc.h"
#include "libft.h"

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
	t_block_type block_type = space->block_type;
	
	pthread_mutex_t* mutex;
	switch (block_type)
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

	size = align_on_16(size);
	if (space->size == size)
	{
		return ptr;
	}
	
	pthread_mutex_lock(mutex);
	if (space->size > size)
	{
		size_t bytes_getting_freed = space->size - size;
		if (!space->is_last)
		{
			t_space *next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + space->size);
			if (!next_space->taken)
			{
				//
				// The next space is free so we merge it with the bytes getting freed by the reallocation
				//
				bytes_getting_freed += next_space->size;

				bool     old_next_space_is_last = next_space->is_last;
				t_space *new_next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + size);

				new_next_space->previous = space;
				new_next_space->size     = bytes_getting_freed;
				new_next_space->taken    = false;
				new_next_space->is_last  = old_next_space_is_last;
				new_next_space->block_type = block_type;

				space->size = size;
				pthread_mutex_unlock(mutex);
				return ptr;
			}
		}
		
		if (bytes_getting_freed >= align_on_16(sizeof(t_space)) + 16)
		{
			//
			// There is no next space or the next space is not free but there are enough remaining bytes to create a new free space
			// If there is a next space, we update its previous pointer to the new free space
			//
			t_space *new_next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + size);

			bool old_space_is_last = space->is_last;
			if (!old_space_is_last)
			{
				t_space *next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + space->size);
				next_space->previous = new_next_space;
			}

			new_next_space->previous   = space;
			new_next_space->size       = bytes_getting_freed;
			new_next_space->taken      = false;
			new_next_space->is_last    = old_space_is_last;
			new_next_space->block_type = block_type;

			space->size    = size;
			space->is_last = true;
			pthread_mutex_unlock(mutex);
			return ptr;
		}

		//
		// There is no next space and the remaining bytes are not big enough to create a new space,
		// we just keep the allocation as it is
		//
		pthread_mutex_unlock(mutex);
		return ptr;
	}
	else
	{
		if (!space->is_last)
		{
			t_space *next_space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + space->size);
			if (!next_space->taken && space->size + align_on_16(sizeof(t_space)) + next_space->size >= size)
			{
				//
				// Next space is free and big enough to accommodate the space new size
				// Is remaining space of the next space big enough to be a new space ?
				// If yes, create it, if not, just merge the next space with the current one
				//
				if (space->size + align_on_16(sizeof(t_space)) + next_space->size >= size + align_on_16(sizeof(t_space)) + 16)
				{
					size_t   old_next_space_size    = next_space->size;
					bool     old_next_space_is_last = next_space->is_last;
					
					t_space *new_next_space      = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + size);
	
					new_next_space->previous   = space;
					new_next_space->size       = old_next_space_size - (size - space->size);
					new_next_space->is_last    = old_next_space_is_last;
					new_next_space->taken      = false;
					new_next_space->block_type = block_type;
	
					space->size = size;
				}
				else
				{
					space->size += align_on_16(sizeof(t_space)) + next_space->size;
					space->is_last = next_space->is_last;
				}
				pthread_mutex_unlock(mutex);
				return ptr;
			}
		}
		
		pthread_mutex_unlock(mutex);
		void *new_ptr = malloc(size);
		if (new_ptr == NULL)
		{
			return NULL;
		}

		pthread_mutex_lock(mutex);
		ft_memcpy(new_ptr, ptr, space->size);
		pthread_mutex_unlock(mutex);

		free(ptr);
		return new_ptr;
	}
	pthread_mutex_unlock(mutex);
	return NULL;
}
