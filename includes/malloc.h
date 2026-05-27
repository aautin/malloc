#pragma once

// LIBC headers
#define _GNU_SOURCE
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>

#define TINY_SIZE  2048
#define SMALL_SIZE (TINY_SIZE * 64)

typedef enum e_block_type
{
	TINY_BLOCK,
	SMALL_BLOCK,
	LARGE_BLOCK,
}	t_block_type;

// Structs
typedef struct s_space
{
	size_t          size;
	struct s_space *previous;
	bool 		    taken;
	bool 		    is_last;
	t_block_type	block_type;
} t_space;

typedef struct s_block
{
	struct s_block *next;
	struct s_block *previous;
} t_block;

typedef struct s_blocks
{
	t_block        *tinies;
	pthread_mutex_t tinies_mutex;

	t_block        *smalls;
	pthread_mutex_t smalls_mutex;

	t_block        *larges;
	pthread_mutex_t larges_mutex;
} t_blocks;

// Utils functions
size_t       get_page_size(void);
size_t       align_on_16(size_t size);
size_t       align_on_page(size_t size);
size_t       get_block_size(t_block_type block_type, size_t size);
size_t       get_large_block_size(size_t custom_size);
void         push_front(t_block **blocks, t_block *new_block);
bool         is_in_blocks(t_block *blocks, t_block *to_find);
size_t       blocks_number(t_block *blocks);
void         remove_block_from_list(t_block **blocks, t_block *to_remove);
t_block_type get_type(size_t size);


// Global variables
t_blocks* get_blocks(void);

// Library functions
void      show_alloc_mem(void);
void      show_alloc_mem_ex(void);
void      free(void *ptr);
void*     malloc(size_t size);
void*     realloc(void *ptr, size_t size);
