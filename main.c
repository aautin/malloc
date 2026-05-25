#include <stdlib.h>
#include <unistd.h>

void write_address(void* ptr)
{
	char buffer[19];
	int  i = 0;

	buffer[0] = '0';
	buffer[1] = 'x';
	for (int j = 2; j < 18; j++)
	{
		int byte = ((size_t)ptr >> (4 * (17 - j))) & 0xF;
		buffer[j] = (byte < 10) ? ('0' + byte) : ('a' + byte - 10);
	}
	buffer[18] = '\n';
	write(STDOUT_FILENO, buffer, 19);
}

int main(void)
{
	void* ptr1 = malloc(42);
	void* ptr2 = malloc(42);

	if (ptr1 == NULL || ptr2 == NULL)
	{
		write(STDERR_FILENO, "Memory allocation failed\n", 25);
		return EXIT_FAILURE;
	}


	write_address(ptr1);
	write_address(ptr2);

	return EXIT_SUCCESS;
}