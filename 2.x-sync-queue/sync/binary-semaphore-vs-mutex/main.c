#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    pthread_mutex_t mutex;
    sem_t sem;
} shmem_locks;

#define handle_err(err, called_func) do {                        \
    if (err) {                                       \
        printf("%s: %s failed: %s\n", __func__, called_func, strerror(err));\
        abort();                                     \
    }                                                \
} while (0)

void *thread_0(void *arg) {
    shmem_locks *locks = (shmem_locks *) arg;
    sem_t *sem = &locks->sem;
    pthread_mutex_t *mutex = &locks->mutex;
    int err, val;

    /* ===================== SEMAPHORE ======================== */
    err = sem_getvalue(sem, &val);
    handle_err(err, "sem_getvalue()");
    printf("[thread_0] semaphore val=%d\n", val);

    err = sem_wait(sem);
    handle_err(err, "sem_wait()");

    err = sem_getvalue(sem, &val);
    handle_err(err, "sem_getvalue()");

    printf("[thread_0] done sem_wait(). val=%d\n", val);

    sleep(2);

    err = sem_getvalue(sem, &val);
    handle_err(err, "sem_getvalue()");
    printf("[thread_0] semaphore val=%d\n", val);


    /* ===================== MUTEX ======================== */
    sleep(4);

    printf("[thread_0] call pthread_mutex_unlock()...\n");
    err = pthread_mutex_unlock(mutex);
    handle_err(err, "pthread_mutex_unlock()");

    return NULL;
}

void *thread_1(void *arg) {
    shmem_locks *locks = (shmem_locks *) arg;
    sem_t *sem = &locks->sem;
    pthread_mutex_t *mutex = &locks->mutex;
    int err, val;


    /* ===================== SEMAPHORE ======================== */
    sleep(1);
    err = sem_getvalue(sem, &val);
    handle_err(err, "sem_getvalue()");

    printf("[thread_1] semaphore val=%d.\n", val);

    err = sem_post(sem);
    handle_err(err, "sem_post()");

    err = sem_getvalue(sem, &val);
    handle_err(err, "sem_getvalue()");

    printf("[thread_1] done sem_post(). val=%d\n", val);
    /* ===================== MUTEX ======================== */
    printf("==========================================\n");
    sleep(2);

    printf("[thread_1]: call mutex_lock()\n");
    err = pthread_mutex_lock(mutex);
    handle_err(err, "pthread_mutex_lock()");

    sleep(3);

    err = pthread_mutex_unlock(mutex);
    handle_err(err, "pthread_mutex_unlock()");
    return NULL;
}


void init_shmem_locks(shmem_locks *ptr) {
    int err;
    assert(ptr);


    pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	assert(err == 0);

	err = pthread_mutex_init(&ptr->mutex, &attr);
	handle_err(err, "pthread_mutex_init()");

	err = sem_init(&ptr->sem, 0, 1);
	handle_err(err, "sem_init()");
}

void destroy_shmem_locks(shmem_locks *ptr) {
    int err;

    err = pthread_mutex_destroy(&ptr->mutex);
    handle_err(err, "pthread_mutex_destroy");

    err = sem_destroy(&ptr->sem);
    handle_err(err, "sem_destroy");
}

int main() {

    int err;
    pthread_t tid[2];

    shmem_locks locks;
    init_shmem_locks(&locks);

    err = pthread_create(&tid[0], NULL, thread_0, &locks);
    if (err) {
        printf("main: pthread_create() for thread_0 failed: %s", strerror(err));
        return 1;
    }

    err = pthread_create(&tid[1], NULL, thread_1, &locks);
    if (err) {
        printf("main: pthread_create() for thread_1 failed: %s", strerror(err));
        return 1;
    }


    err = pthread_join(tid[0], NULL);
    if (err) {
        printf("main: pthread_join() for thread_0 failed: %s", strerror(err));
        return 1;
    }

    err = pthread_join(tid[1], NULL);
    if (err) {
        printf("main: pthread_join() for thread_1 failed: %s", strerror(err));
        return 1;
    }

    destroy_shmem_locks(&locks);

    return 0;
}
