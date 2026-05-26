#include "malloc.h"
#include "libft.h"

static void print_address(void* address)
{
	char buffer[20];
	int i = 0;
	unsigned long long addr = (unsigned long long) address;
	
	if (addr == 0)
	{
		write(STDOUT_FILENO, "0x0", 3);
		return;
	}
	
	while (addr > 0)
	{
		int digit = addr % 16;
		if (digit < 10)
		buffer[i++] = '0' + digit;
		else
		buffer[i++] = 'a' + (digit - 10);
		addr /= 16;
	}
	
	write(STDOUT_FILENO, "0x", 2);
	while (i > 0)
	{
		write(STDOUT_FILENO, &buffer[--i], 1);
	}
}

static size_t print_block(t_block *block)
{
	size_t bytes = 0;
	
	t_space *space = (t_space *)((char *)block + align_on_16(sizeof(t_block)));
	while (true)
	{
		if (space->taken)
		{
			print_address((char *)space + align_on_16(sizeof(t_space)));
			write(STDOUT_FILENO, " - ", 3);
			print_address((char *)space + align_on_16(sizeof(t_space)) + space->size);
			write(STDOUT_FILENO, " : ", 3);
			ft_putnbr_fd((int)space->size, STDOUT_FILENO);
			write(STDOUT_FILENO, " bytes\n", 7);
			bytes += space->size;
		}
		if (space->is_last)
		{
			break;
		}
		space = (t_space *)((char *)space + align_on_16(sizeof(t_space)) + space->size);
	}

	return bytes;
}

static void print_block_header(char* type, void* address)
{
	ft_putstr_fd(type, STDOUT_FILENO);
	ft_putstr_fd(" : ", STDOUT_FILENO);
	
	print_address(address);

	write(STDOUT_FILENO, "\n", 1);
}

void show_alloc_mem(void)
{
	size_t bytes = 0;
	for (t_block *block = get_blocks()->tinies; block != NULL; block = block->next)
	{
		print_block_header("TINY", block);
		bytes += print_block(block);
	}

	for (t_block *block = get_blocks()->smalls; block != NULL; block = block->next)
	{
		print_block_header("SMALL", block);
		bytes += print_block(block);
	}

	for (t_block *block = get_blocks()->larges; block != NULL; block = block->next)
	{
		print_block_header("LARGE", block);
		bytes += print_block(block);
	}

	ft_putstr_fd("Total : ", STDOUT_FILENO);
	ft_putnbr_fd((int)bytes, STDOUT_FILENO);
	write(STDOUT_FILENO, " bytes\n", 7);
}
