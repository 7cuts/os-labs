#include <stdio.h>
#include "uthread.h"
#include "uthread_impl.h"
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int cnt;

void *start_func(void *arg) {
    UTHREAD_PRINTF("start_func() entry");
    int cnt = 0;
    for (int i = 0; i < 10; i++) {
        cnt++;
        uthread_yield();
    }

    UTHREAD_PRINTF("returning...");
    return (void *) (uintptr_t)cnt;
}

int main() {
    printf("my pid = %d\n", getpid());
    uthread_t thread;
    int r =  uthread_create(&thread, start_func, NULL, "th0");
    if (r != 0) {
        errno = -r;
        err(EXIT_FAILURE, "uthread_create");
    }
    UTHREAD_PRINTF("uthread created");

    intptr_t retcnt;
    UTHREAD_PRINTF("call uthread_join()");
    uthread_join(thread, (void*)&retcnt);
    if (r != 0) {
        errno = -r;
        err(EXIT_FAILURE, "uthread_join");
    }

    UTHREAD_PRINTF("got retcnt = %ld", retcnt);
}
