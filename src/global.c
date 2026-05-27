#include "malloc.h"


t_blocks g_blocks = {0};

__attribute__((constructor)) void init_global()
{
	pthread_mutex_init(&g_blocks.tinies_mutex, NULL);
	pthread_mutex_init(&g_blocks.smalls_mutex, NULL);
	pthread_mutex_init(&g_blocks.larges_mutex, NULL);
}

t_blocks* get_blocks()
{
	return &g_blocks;
}
