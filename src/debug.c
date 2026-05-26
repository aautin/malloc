#include "malloc.h"
#include "libft.h"

static void show_spaces(t_block *blocks)
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
	for (t_block *block = get_blocks()->tinies; block != NULL; block = block->next)
	{
		write(STDOUT_FILENO, "Tiny block:\n", 12);
		show_spaces(block);
	}

	for (t_block *block = get_blocks()->smalls; block != NULL; block = block->next)
	{
		write(STDOUT_FILENO, "Small block:\n", 13);
		show_spaces(block);
	}

	for (t_block *block = get_blocks()->larges; block != NULL; block = block->next)
	{
		write(STDOUT_FILENO, "Large block:\n", 13);
		show_spaces(block);
	}

	write(STDOUT_FILENO, "-------------------------\n\n", 26);
}
