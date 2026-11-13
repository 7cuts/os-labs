#include "../test.h"
#include <asm-generic/errno-base.h>
#include <string.h>

static void *
start_func(void *arg)
{

}

TEST(test_join_null) {

    uthread_t thread;
    int r = uthread_create(&thread, start_func, (void *)21, "thread");
    if (r != 0) {
        FAIL("uthread create failed: %s", strerror(-r));
    }

    r = uthread_join((uthread_t)NULL, NULL);
    if (r != -EINVAL) {
        FAIL("uthread_join: expected got error = %s, but got %s", strerror(EINVAL), strerror(-r));
    }

    // do not forget to remove thread... should not fail
    r = uthread_join(thread, NULL);
    if (r != 0) {
        FAIL("uthread join failed: %s", strerror(-r));
    }
    OK();
}
