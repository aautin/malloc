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

static size_t print_block(t_block *block, bool show_dump)
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

			for (size_t i = 0; show_dump && i < space->size; i+=16)
			{
				unsigned char *data = (unsigned char *)((char *)space + align_on_16(sizeof(t_space)));
				size_t line_size = (space->size - i) < 16 ? (space->size - i) : 16;
				char buf[64];
				int pos = 0;
				/* offset */
				unsigned int off = (unsigned int)i;
				for (int k = 7; k >= 0; k--)
				{
					int nib = (off >> (k * 4)) & 0xF;
					buf[pos++] = (nib < 10) ? ('0' + nib) : ('a' + (nib - 10));
				}
				buf[pos++] = '\t';
				/* hex bytes */
				for (size_t j = 0; j < 16; j++)
				{
					if (j < line_size)
					{
						unsigned char v = data[i + j];
						int hi = (v >> 4) & 0xF;
						int lo = v & 0xF;
						buf[pos++] = (hi < 10) ? ('0' + hi) : ('a' + (hi - 10));
						buf[pos++] = (lo < 10) ? ('0' + lo) : ('a' + (lo - 10));
					}
					else
					{
						buf[pos++] = '0'; buf[pos++] = '0';
					}
					if (j != 15) buf[pos++] = ' ';
				}
				/* spacer and ascii */
				buf[pos++] = ' ';
				buf[pos++] = ' ';
				buf[pos++] = '|';
				for (size_t j = 0; j < line_size; j++)
				{
					unsigned char c = data[i + j];
					buf[pos++] = (c >= 32 && c <= 126) ? c : '.';
				}
				/* fill remaining ascii slots */
				for (size_t j = line_size; j < 16; j++) buf[pos++] = '.';
				buf[pos++] = '|';
				buf[pos++] = '\n';
				write(STDOUT_FILENO, buf, pos);
			}
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
		bytes += print_block(block, false);
	}

	for (t_block *block = get_blocks()->smalls; block != NULL; block = block->next)
	{
		print_block_header("SMALL", block);
		bytes += print_block(block, false);
	}

	for (t_block *block = get_blocks()->larges; block != NULL; block = block->next)
	{
		print_block_header("LARGE", block);
		bytes += print_block(block, false);
	}

	ft_putstr_fd("Total : ", STDOUT_FILENO);
	ft_putnbr_fd((int)bytes, STDOUT_FILENO);
	write(STDOUT_FILENO, " bytes\n", 7);
}

void show_alloc_mem_ex(void)
{
	size_t bytes = 0;
	for (t_block *block = get_blocks()->tinies; block != NULL; block = block->next)
	{
		print_block_header("TINY", block);
		bytes += print_block(block, true);
	}

	for (t_block *block = get_blocks()->smalls; block != NULL; block = block->next)
	{
		print_block_header("SMALL", block);
		bytes += print_block(block, true);
	}

	for (t_block *block = get_blocks()->larges; block != NULL; block = block->next)
	{
		print_block_header("LARGE", block);
		bytes += print_block(block, true);
	}

	ft_putstr_fd("Total : ", STDOUT_FILENO);
	ft_putnbr_fd((int)bytes, STDOUT_FILENO);
	write(STDOUT_FILENO, " bytes\n", 7);
}