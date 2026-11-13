#include "../test.h"
#include <string.h>

static void *
start_func(void *arg)
{
    intptr_t num = (intptr_t)arg;
    return (void *) (num * 2);
}


TEST(test_arg) {

    uthread_t thread;
    int r = uthread_create(&thread, start_func, (void *)21, "thread");
    if (r != 0) {
        FAIL("uthread create failed: %s", strerror(-r));
    }

    intptr_t retval;
    r = uthread_join(thread, (void*) &retval);
    if (r != 0) {
        FAIL("uthread join failed: %s", strerror(-r));
    }

    if (retval != 42) {
        FAIL("got retval = %ld, expected 42", retval);
    }

    OK();
}
