#include "malloc.h"


t_blocks g_blocks = {NULL, NULL, NULL, {0, 0}};

__attribute__((constructor)) void init_global()
{
	gettimeofday(&g_blocks.start_time, NULL);
}

t_blocks* get_blocks()
{
	return &g_blocks;
}
