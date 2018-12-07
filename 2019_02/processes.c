#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <signal.h>
#include <time.h>

#define NSECS_PER_SEC 1000000000UL
#define NPROCESS 10000

static inline long diff_nsec(struct timespec before, struct timespec after)
{
        return ((after.tv_sec * NSECS_PER_SEC + after.tv_nsec)
                - (before.tv_sec * NSECS_PER_SEC + before.tv_nsec));
}

static void process_fn(void)
{
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	if (argc != 2) {
		fprintf(stderr, "usage: %s <load[MB]>\n",
			progname);
		exit(EXIT_FAILURE);
	}
	int n = NPROCESS;
	int load = atoi(argv[1]);
	char *buf = malloc(1024 * 1024 * load);
	int i;
	for (i = 0; i < load * 1024 * 1024; i += 4 * 1024)
		buf[i] = 0;
	struct timespec before, after;
	long sum = 0;
	for (i = 0; i < n; i++) {
		pid_t pid;
		clock_gettime(CLOCK_MONOTONIC, &before);
		pid = fork();
		if (pid == -1)
			err(EXIT_FAILURE, "fork() failed");
		else if (pid == 0)
			process_fn();
		clock_gettime(CLOCK_MONOTONIC, &after);
		sum += diff_nsec(before, after);
		wait(NULL);
	}
	printf("%ld[ns]\n", sum/n);
	exit(EXIT_SUCCESS);
}
