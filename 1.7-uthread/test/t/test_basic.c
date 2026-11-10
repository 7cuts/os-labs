#include "../test.h"

int cnt;

void *
start_func(void *arg)
{
    for (int i = 0; i < 10; i++) {
        cnt++;
        uthread_yield();
    }
}


TEST(test_basic) {

    uthread_t thread;
    for (int i = 0; i < 10; i++) {
        char name[32] = {0};
        snprintf(name, sizeof(name), "counter-%d", i);
        int r = uthread_create(&thread, start_func, NULL, name);

        if (r != 0)
        {
            FAIL("cannot create thread 'counter-%d'", i);
        }

    }
    OK();
}
