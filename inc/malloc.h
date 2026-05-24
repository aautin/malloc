#pragma once

// LIBC headers
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

enum e_fixed_size
{
	TINY  = 2048,
	SMALL = TINY * 128,
};

// Structs
typedef struct s_space
{
	size_t          size;
	struct s_space *previous;
	bool 		    taken;
}	t_space;

typedef struct s_block
{
	struct s_block *next;
}	t_block;

typedef struct s_large_block
{
	size_t                size;
	struct s_large_block *next;
}	t_large_block;
//
// One single space is placed right after the block metadata (start and end 16-bytes aligned)
// No size (already in block metadata), no previous pointer and no taken boolean (a large block is always alone and taken)
//

typedef struct s_blocks
{
	t_block       *tinies;
	t_block       *smalls;
	t_large_block *larges;
}	t_blocks;

// Functions
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
