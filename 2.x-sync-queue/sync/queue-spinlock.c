#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>
#include <assert.h>

#define QUEUE_SYNC_SPINLOCK
#include "queue.h"

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		queue_print_stats(q);
		sleep(1);
	}

	return NULL;
}

queue_t* queue_init(int max_count) {
	int err;

	queue_t *q = malloc(sizeof(queue_t));
	if (!q) {
		printf("Cannot allocate memory for a queue\n");
		abort();
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	err = pthread_spin_init(&q->lock, PTHREAD_PROCESS_PRIVATE);
	if (err) {
		printf("queue_init: pthread_spin_init() failed: %s\n", strerror(err));
		abort();
	}

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		abort();
	}

	return q;
}

void queue_destroy(queue_t *q) {
    int err;

    if (!q)  {
        return;
    }

    err = pthread_cancel(q->qmonitor_tid);
    assert(err == 0);

    err = pthread_join(q->qmonitor_tid, NULL);
    if (err) {
        printf("Cannot destroy queue: could not join qmonitor thread: %s\n", strerror(err));
        abort();
    }

    qnode_t *node = q->first;
    while (node) {
        qnode_t *next = node->next;
        free(node);
        node = next;
    }

    // not necessary but it is good tone i think
    q->first = NULL;
    q->last = NULL;
    q->count = 0;
    err = pthread_spin_trylock(&q->lock);
    if (err) {
        // Somebody is holding the lock. According
        // to man pthread_spin_destroy(3): destroying
        // locked lock is UB.
        printf("Cannot destroy queue: somebody is holding the lock: %s\n", strerror(err));
        abort();
    }

    pthread_spin_unlock(&q->lock);
    pthread_spin_destroy(&q->lock);


    free(q);

    printf(":: queue freed\n");
}

int queue_add(queue_t *q, int val) {
	q->add_attempts++;

	assert(q->count <= q->max_count);

	qnode_t *new = malloc(sizeof(qnode_t));
	if (!new) {
		printf("Cannot allocate memory for new node\n");
		abort();
	}
	new->val = val;
	new->next = NULL;

	pthread_spin_lock(&q->lock);
	if (q->count == q->max_count) {
	    pthread_spin_unlock(&q->lock);
	    return 0;
	}

	if (!q->first) {
        //printf("writer: queue_add(): q->first == NULL\n");
        q->first = q->last = new;
	}
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	q->count++;
	pthread_spin_unlock(&q->lock);

	q->add_count++;

	return 1;
}

int queue_get(queue_t *q, int *val) {
	q->get_attempts++;

	assert(q->count >= 0);

	pthread_spin_lock(&q->lock);
	if (q->count == 0) {
	    pthread_spin_unlock(&q->lock);
	    return 0;
	}

	qnode_t *tmp = q->first;

	*val = tmp->val;
	q->first = q->first->next;

	q->count--;
	q->get_count++;
	pthread_spin_unlock(&q->lock);

	free(tmp);
	return 1;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}
