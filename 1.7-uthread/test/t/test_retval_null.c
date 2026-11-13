
#include "../test.h"
#include <string.h>

static void *
start_func(void *arg)
{
    intptr_t num = (intptr_t)arg;
    return (void *) (num * 2);
}


TEST(test_retval_null) {

    uthread_t thread;
    int r = uthread_create(&thread, start_func, (void *)21, "thread");
    if (r != 0) {
        FAIL("uthread create failed: %s", strerror(-r));
    }

    r = uthread_join(thread, NULL);
    if (r != 0) {
        FAIL("uthread join failed: %s", strerror(-r));
    }
    OK();
}
