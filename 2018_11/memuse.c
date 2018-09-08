#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PAGE_SIZE 4096

int main(int argc, char *argv[]) {
	char *progname = argv[0];
	if (argc != 2) {
		fprintf(stderr, "usage: %s <size>\n", progname);
		exit(EXIT_FAILURE);
	}
	int size = atoi(argv[1]);
	char *buf = malloc(size);
	if (!buf) {
		fprintf(stderr, "malloc() failed");
		exit(EXIT_FAILURE);
	}
	int i;
	for (i = 0; i < size; i += PAGE_SIZE)
		buf[i] = 0;
	pause();
	free(buf);
}
