#pragma once

// LIBC headers
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>

enum e_fixed_size
{
	TINY  = 2048,
	SMALL = TINY * 64,
};

// Structs
typedef struct s_space
{
	size_t          size;
	struct s_space *previous;
	char 		    taken;
	char 		    is_last;
}	t_space;

typedef struct s_block
{
	struct s_block *next;
	struct s_block *previous;
}	t_block;

typedef struct s_blocks
{
	t_block *tinies;
	t_block *smalls;
	t_block *larges;
}	t_blocks;

// Utils functions
size_t get_page_size(void);
size_t align_on_16(size_t size);
size_t align_on_page(size_t size);
size_t get_block_size(enum e_fixed_size fixed_size);
size_t get_large_block_size(size_t custom_size);
void   push_front(t_block **blocks, t_block *new_block);
bool   is_in_blocks(t_block *blocks, t_block *to_find);
size_t blocks_number(t_block *blocks);
void   remove_block_from_list(t_block **blocks, t_block *to_remove);

// Global variables
t_blocks* get_blocks(void);

// Library functions
void      show_memory(void);
void      free(void *ptr);
void*     malloc(size_t size);
void*     realloc(void *ptr, size_t size);
