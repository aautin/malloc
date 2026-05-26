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
	size = align_on_16(size);
	if (space->size == size)
	{
		return ptr;
	}
	else if (space->size > size)
	{
		//
		// To be continued
		//
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
	
					new_next_space->previous = space;
					new_next_space->size     = old_next_space_size - (size - space->size);
					new_next_space->is_last  = old_next_space_is_last;
					new_next_space->taken    = false;
	
					space->size = size;
				}
				else
				{
					space->size += align_on_16(sizeof(t_space)) + next_space->size;
					space->is_last = next_space->is_last;
				}
				return ptr;
			}
		}

		void *new_ptr = malloc(size);
		if (new_ptr == NULL)
		{
			return NULL;
		}

		ft_memcpy(new_ptr, ptr, space->size);
		free(ptr);
		return new_ptr;
	}
	return (NULL);
}
