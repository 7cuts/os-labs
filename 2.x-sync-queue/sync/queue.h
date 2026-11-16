#ifndef __FITOS_QUEUE_H__
#define __FITOS_QUEUE_H__

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _QueueNode {
	int val;
	struct _QueueNode *next;
} qnode_t;

typedef struct _Queue {

#ifdef QUEUE_SYNC_SPINLOCK
    pthread_spinlock_t lock;
#endif
#ifdef QUEUE_SYNC_MUTEX
    pthread_mutex_t mutex;
#endif
#ifdef QUEUE_SYNC_CONDVAR
    pthread_mutex_t mutex;
    // need both full and empty conditions if use more than 2 threads
    pthread_cond_t cond_queue_full;
    pthread_cond_t cond_queue_empty;
#endif
	qnode_t *first;
	qnode_t *last;

	pthread_t qmonitor_tid;

	int count;
	int max_count;

	// queue statistics
	long add_attempts;
	long get_attempts;
	long add_count;
	long get_count;
} queue_t;

queue_t* queue_init(int max_count);
void queue_destroy(queue_t *q);
int queue_add(queue_t *q, int val);
int queue_get(queue_t *q, int *val);
void queue_print_stats(queue_t *q);

#endif		// __FITOS_QUEUE_H__
