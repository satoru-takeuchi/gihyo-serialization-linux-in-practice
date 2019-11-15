#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define NLOOP_FOR_ESTIMATION 1000000000UL
#define NSECS_PER_MSEC 1000000UL
#define NSECS_PER_SEC 1000000000UL

static inline long diff_nsec(struct timespec before, struct timespec after)
{
        return ((after.tv_sec * NSECS_PER_SEC + after.tv_nsec)
                - (before.tv_sec * NSECS_PER_SEC + before.tv_nsec));
}

static inline unsigned long loops_per_msec()
{
        struct timespec before, after;
        clock_gettime(CLOCK_MONOTONIC, &before);

        unsigned long i;
        for (i = 0; i < NLOOP_FOR_ESTIMATION; i++)
		;

        clock_gettime(CLOCK_MONOTONIC, &after);

	int ret;
        return  NLOOP_FOR_ESTIMATION * NSECS_PER_MSEC / diff_nsec(before, after);
}
 
static inline void load(unsigned long nloop)
{
        unsigned long i;
        for (i = 0; i < nloop; i++)
                ;
}

static void child_fn(int id, struct timespec *buf, int total, unsigned long loops_per_msec, struct timespec start)
{
        int i;
        for (i = 0; i < total; i++) {
                struct timespec ts;

                load(loops_per_msec);
                clock_gettime(CLOCK_MONOTONIC, &ts);
                buf[i] = ts;
        }
        for (i = 0; i < total; i++) {
                printf("%d\t%ld\t%d\n", id, diff_nsec(start, buf[i]) / NSECS_PER_MSEC, (i + 1) * 100 / total);
        }
        exit(EXIT_SUCCESS);
}
 
static void parent_fn(int nproc)
{
        int i;
        for (i = 0; i < nproc; i++)
                wait(NULL);
}

int main(int argc, char *argv[])
{
        int ret = EXIT_FAILURE;

        if (argc < 2) {
                fprintf(stderr, "usage: %s <nproc>\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        int nproc = atoi(argv[1]);
        if (nproc < 1) {
                fprintf(stderr, "<nproc>(%d) should be >= 1\n", nproc);
                exit(EXIT_FAILURE);
        }
	int total = 100;

        struct timespec *logbuf = malloc(total * sizeof(struct timespec));

	puts("estimating workload taking one milisecond");
        unsigned long loops = loops_per_msec();
	puts("end estimation");
	puts("press enter to start");
	getchar();

	pid_t *pids;
        pids = malloc(nproc * sizeof(pid_t));

        struct timespec start;
        clock_gettime(CLOCK_MONOTONIC, &start);

        int i, ncreated;
        for (i = 0, ncreated = 0; i < nproc; i++, ncreated++) {
                pids[i] = fork();
                if (pids[i] < 0) {
			int j;
			for (j = 0; j < ncreated; i++)
				kill(pids[j], SIGINT);
                        goto wait_children;
                } else if (pids[i] == 0) {
                        // children
                        child_fn(i, logbuf, total, loops, start);
                        /* shouldn't reach here */
			abort();
                }
        }

        // parent
        ret = EXIT_SUCCESS;

wait_children:
        for (i = 0; i < ncreated; i++)
                wait(NULL);

free_pids:
        free(pids);
free_logbuf:
        free(logbuf);

        exit(ret);
}
