#define _GNU_SOURCE
#include <semaphore.h>
#include <sched.h>
#include <pthread.h>
#include <assert.h>

#define QUEUE_SYNC_SEMAPHORE
#include "queue.h"

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
	    pthread_mutex_lock(&q->mutex);
		queue_print_stats(q);
		pthread_mutex_unlock(&q->mutex);

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
	err = sem_init(&q->count, 0 /* not pshared */, 0 /* init value */);
	if (err) {
		printf("queue_init: sem_init(&q->count) failed: %s\n", strerror(err));
		abort();
	}

	err = sem_init(&q->free, 0 /* not pshared */, max_count /* init value */);
	if (err) {
		printf("queue_init: sem_init(&q->free) failed: %s\n", strerror(err));
		abort();
	}

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;



	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	assert(err == 0);

	err = pthread_mutex_init(&q->mutex, &attr);
	if (err) {
		printf("queue_init: pthread_mutex_init() failed: %s\n", strerror(err));
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
    err = pthread_mutex_trylock(&q->mutex);
    if (err) {
        // Somebody is holding the mutex. Strange ....
        printf("Cannot destroy queue: somebody is holding the mutex: %s", strerror(err));
        abort();
    }
    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);

    err = sem_destroy(&q->count);
    assert(!err);

    err = sem_destroy(&q->free);
    assert(!err);

    free(q);

    printf(":: queue freed\n");
}

int queue_add(queue_t *q, int val) {
	q->add_attempts++;


	qnode_t *new = malloc(sizeof(qnode_t));
	if (!new) {
		printf("Cannot allocate memory for new node\n");
		abort();
	}
	new->val = val;
	new->next = NULL;


	sem_wait(&q->free);

	int cnt;
	sem_getvalue(&q->count, &cnt);
	assert(cnt < q->max_count); /* cnt never should be equals to q->max_count */

	pthread_mutex_lock(&q->mutex);

	if (!q->first) {
        //printf("writer: queue_add(): q->first == NULL\n");
        q->first = q->last = new;
	}
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->count);

	q->add_count++;

	return 1;
}

int queue_get(queue_t *q, int *val) {
	q->get_attempts++;

	sem_wait(&q->count);

	// We shure that we have at least 1 node to get
	// and nobody steals it from us.
	int cnt;
	sem_getvalue(&q->count, &cnt);
	assert(cnt >= 0);

	pthread_mutex_lock(&q->mutex);

	qnode_t *tmp = q->first;
	*val = tmp->val;
	q->first = q->first->next;

	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->free);

	q->get_count++;
	free(tmp);
	return 1;
}

void queue_print_stats(queue_t *q) {
    int cnt;
    sem_getvalue(&q->count, &cnt);
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		cnt,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}
