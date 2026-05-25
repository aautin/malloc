#pragma once

// LIBC headers
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

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

// Functions
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
