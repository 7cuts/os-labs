#include "../test.h"
#include <string.h>

static void *
start_func(void *arg)
{
    /*  this thing uses `name` field. it just should not segfault */
    uthread_printf("i'am in start_func");
    return NULL;
}


TEST(test_name_null) {

    uthread_t thread;
    int r = uthread_create(&thread, start_func, NULL, NULL);
    if (r != 0) {
        FAIL("uthread create failed: %s", strerror(-r));
    }

    r = uthread_join(thread, NULL);
    if (r != 0) {
        FAIL("uthread join failed: %s", strerror(-r));
    }
    OK();
}
