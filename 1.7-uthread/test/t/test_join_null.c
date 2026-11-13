#include "../test.h"
#include <asm-generic/errno-base.h>
#include <string.h>

void start_func() {

}

TEST(test_create_start_func_null) {

    uthread_t thread;
    int r = uthread_create(&thread, NULL, (void *)21, "thread");
    if (r != -EINVAL) {
        FAIL("uthread_create: expected got error = %s, but got %s", strerror(EINVAL), strerror(-r));
    }

    r = uthread_join(NULL, NULL);
    if (r != -EINVAL) {
        FAIL("uthread_join: expected got error = %s, but got %s", strerror(EINVAL), strerror(-r));
    }

    // do not forget to remove thread... should not fail
    if (r != 0) {
        FAIL("uthread join failed: %s", strerror(-r));
    }
    OK();
}
