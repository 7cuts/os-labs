#define _GNU_SOURCE
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>
#include <sched.h>

#include "queue.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

#define WORKING_TIME_SECONDS 10

void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset);
	CPU_SET(n, &cpuset);

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
	if (err) {
		printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
		return;
	}

	printf("set_cpu: set cpu %d\n", n);
}

void *reader(void *arg) {
	int expected = 0;
	queue_t *q = (queue_t *)arg;
	printf("reader [%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(1);

	while (1) {
		int val = -1;
		int ok = queue_get(q, &val);
		if (!ok)
			continue;

		if (expected != val)
			printf(RED"ERROR: get value is %d but expected - %d" NOCOLOR "\n", val, expected);

		expected = val + 1;
	}

	return NULL;
}

void *writer(void *arg) {
	int i = 0;
	queue_t *q = (queue_t *)arg;
	printf("writer [%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(12);

	while (1) {
		int ok = queue_add(q, i);
		if (!ok)
			continue;
		i++;

		usleep(1);
	}

	return NULL;
}

int parse_args(int argc, char **argv) {

    if (argc != 2) {
        printf("usage:\n\t %s <queue size>\n", argv[0]);
        exit(1);
    }

    char *endptr = NULL;
    long queue_sz = strtol(argv[1], &endptr, 10);

    bool valid = endptr != NULL && *endptr == '\0' && argv[1] != NULL;
    if (queue_sz <= 0 || queue_sz > INT_MAX ||!valid) {
        printf("parse_args: bad queue size or it is out of integer range.\n");
        exit(2);
    }

    return queue_sz;
}

int main(int argc, char **argv) {
    pthread_t reader_tid;
    pthread_t writer_tid;
	queue_t *q;
	int err;

	printf("main [%d %d %d]\n", getpid(), getppid(), gettid());
	q = queue_init(parse_args(argc, argv));

	err = pthread_create(&reader_tid, NULL, reader, q);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	sched_yield();

	err = pthread_create(&writer_tid, NULL, writer, q);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	sleep(WORKING_TIME_SECONDS);

	printf("main: canceling threads...\n");

	err = pthread_cancel(reader_tid);
	assert(!err);

	err = pthread_cancel(writer_tid);
	assert(!err);

	puts("main: joining reader...");
	void *retval;
	err = pthread_join(reader_tid, &retval);
	if (err) {
    	printf("main: pthread_join(reader_tid) failed: %s\n", strerror(err));
    	return -1;
	}
	assert(retval == PTHREAD_CANCELED);

	puts("main: joining reader complete");
	puts("main: joining writer...");
	err = pthread_join(writer_tid, &retval);
	if (err) {
    	printf("main: pthread_join(writer_tid) failed: %s\n", strerror(err));
    	return -1;
	}
	assert(retval == PTHREAD_CANCELED);
	puts("main: joining writer complete");


	printf("get_count: %ld\n", q->get_count);
	printf("add_count: %ld\n", q->add_count);

	puts("main: destroing queue...");
	queue_destroy(q);

	pthread_exit(NULL);

	return 0;
}
