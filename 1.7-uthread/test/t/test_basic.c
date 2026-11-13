#include "../test.h"
#include <string.h>

static int cnt;

static void *
start_func(void *arg)
{
    for (int i = 0; i < 10; i++) {
        cnt++;
        uthread_yield();
    }
}

#define NUM_THREADS 10
#define ITERS 10

TEST(test_basic) {
    uthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        char name[32] = {0};
        snprintf(name, sizeof(name), "counter-%d", i);
        int r = uthread_create(&threads[i], start_func, NULL, name);

        if (r != 0) {
            FAIL("cannot create thread 'counter-%d': %s", i, strerror(-r));
        }
    }


    for (int i = 0; i < NUM_THREADS; i++) {
        int r = uthread_join(threads[i], NULL);

        if (r != 0) {
            FAIL("cannot join thread 'counter-%d': %s", i, strerror(-r));
        }
    }

    if (cnt != NUM_THREADS * ITERS) {
        FAIL("expected cnt == %d but got %d", 10 * 10, cnt);
    }
    OK();
}
