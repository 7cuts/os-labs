#include "../test.h"
#include <asm-generic/errno-base.h>
#include <string.h>

static void *
start_func(void *arg)
{

}

TEST(test_create_null) {

    int r = uthread_create(NULL, start_func, (void *)21, "thread");
    if (r != -EINVAL) {
        FAIL("uthread_create: expected got error = %s, but got %s", strerror(EINVAL), strerror(-r));
    }
    OK();
}
