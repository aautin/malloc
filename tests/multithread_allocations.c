#include "malloc.h"
#include "libft.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//
// Each thread does 1 tiny, 1 small, 1 large allocations
//
void *thread_mixed_allocations(void *arg)
{
	int thread_id = *(int *)arg;
	size_t allocations[3] = {256, 4096, 131072};

	void** allocations_results = malloc(sizeof(void*) * 3);

	for (size_t i = 0; i < 3; ++i)
	{
		allocations_results[i] = malloc(allocations[i]);
		if (allocations_results[i] == NULL)
		{
			perror("malloc failed");
			exit(EXIT_FAILURE);
		}
	}

	return allocations_results;
}

int main(void)
{
	int thread_ids[3] = {1, 2, 3};
	pthread_t threads[3];	
	void** thread_results[3];  // Array to store the return values from each thread

	write(STDOUT_FILENO, "Starting multithreaded memory allocation test...\n", 48);

	
	for (int i = 0; i < 3; i++)
	{
		pthread_create(&threads[i], NULL, thread_mixed_allocations, &thread_ids[i]);
	}
	
	
	for (int i = 0; i < 3; i++)
	{
		pthread_join(threads[i], (void**)&thread_results[i]);
		
		free(thread_results[i][2]);
	}

	ft_putnbr_fd(3 * 256 + 3 * 4096, STDOUT_FILENO);
	write(STDOUT_FILENO, " bytes will be allocated in total by the threads.\n", 50);
	
	write(STDOUT_FILENO, "\n=== MEMORY STATE AFTER ALLOCATIONS IN THREADS ===\n", 51);
	show_alloc_mem_ex();

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			free(thread_results[i][j]);
		}
		free(thread_results[i]);
	}

	write(STDOUT_FILENO, "\n=== MEMORY STATE AFTER FREEING ALL ALLOCATIONS ===\n", 53);
	show_alloc_mem_ex();

	return 0;
}
