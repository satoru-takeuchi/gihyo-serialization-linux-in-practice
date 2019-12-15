#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#define BUFFER_SIZE	(128 * 1024 * 1024)
#define PAGE_SIZE	4096

int main(void)
{
	char *p;
	time_t t;
	char *s;

	printf("Before allocation, please press Enter key\n");
	getchar();

	p = malloc(BUFFER_SIZE);
	if (p == NULL)
		err(EXIT_FAILURE, "malloc() failed");

	printf("Allocated 128MiB, please press Enter key\n");
	getchar();

	int i;
	for (i = 0; i < BUFFER_SIZE; i += PAGE_SIZE) {
		p[i] = 1;
	}

	printf("Touched 128MiB, please press Enter key\n");
	getchar();

	exit(EXIT_SUCCESS);
}
