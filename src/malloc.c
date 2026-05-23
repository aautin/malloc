#include "malloc.h"

#include <unistd.h>

void *malloc(size_t size)
{
	write(STDOUT_FILENO, "My malloc called\n", 18);

	(void) size;
	
	return (NULL);
}
