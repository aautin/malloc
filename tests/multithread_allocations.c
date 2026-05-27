#include "malloc.h"
#include "libft.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

//
// Each thread does 1 tiny and 1 small allocations
//
void *thread_mixed_allocations(void *arg)
{
	(void) arg;
	size_t allocations[2] = {256, 4096};

	void** allocations_results = malloc(sizeof(void*) * 2);

	for (size_t i = 0; i < 2; ++i)
	{
		allocations_results[i] = malloc(allocations[i]);
	}

	return allocations_results;
}

//
// Each thread reallocs its tiny and small allocations to smaller and bigger sizes, respectively
//
void *thread_mixed_reallocations(void *arg)
{
	void** allocations_results = arg;
	size_t allocations[2] = {20, 4500};

	for (size_t i = 0; i < 2; ++i)
	{
		allocations_results[i] = realloc(allocations_results[i], allocations[i]);
	}

	return allocations_results;
}

//
// Each thread free its tiny and small allocations
//
void *thread_mixed_frees(void *arg)
{
	void** allocations_results = (void**) arg;

	for (size_t i = 0; i < 2; ++i)
	{
		free(allocations_results[i]);
	}

	return NULL;
}

int main(void)
{
	int thread_ids[3] = {1, 2, 3};
	pthread_t threads[3];	
	void** thread_results[3];  // Array to store the return values from each thread

	// -------------------------------------- ALLOCATIONS --------------------------------------//
	for (int i = 0; i < 3; i++)
	{
		pthread_create(&threads[i], NULL, thread_mixed_allocations, &thread_ids[i]);
	}
	
	for (int i = 0; i < 3; i++)
	{
		pthread_join(threads[i], (void**)&thread_results[i]);
	}
	
	write(STDOUT_FILENO, "\n=== MEMORY STATE AFTER ALLOCATIONS IN THREADS ===\n", 52);
	show_alloc_mem();

	// -------------------------------------- REALLOCATIONS --------------------------------------//
	for (int i = 0; i < 3; i++)
	{
		pthread_create(&threads[i], NULL, thread_mixed_reallocations, thread_results[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		pthread_join(threads[i], (void**)&thread_results[i]);
	}

	write(STDOUT_FILENO, "\n=== MEMORY STATE AFTER REALLOCATIONS IN THREADS ===\n", 51);
	show_alloc_mem();

	// -------------------------------------- FREES --------------------------------------//
	for (int i = 0; i < 3; i++)
	{
		pthread_create(&threads[i], NULL, thread_mixed_frees, thread_results[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		pthread_join(threads[i], NULL);
	}
	
	write(STDOUT_FILENO, "\n=== MEMORY STATE AFTER FREEING IN THREADS ===\n", 47);
	show_alloc_mem();


	for (int i = 0; i < 3; i++)
	{
		free(thread_results[i]);
	}

	write(STDOUT_FILENO, "\n=== MEMORY STATE AFTER FREEING THREAD RESULTS ===\n", 52);
	show_alloc_mem();

	return 0;
}
