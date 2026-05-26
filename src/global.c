#include "malloc.h"

t_blocks g_blocks = {NULL, NULL, NULL};

t_blocks* get_blocks()
{
	return &g_blocks;
}
