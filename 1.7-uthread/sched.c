#include <assert.h>
#include <err.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ucontext.h>
#include <unistd.h>

#include "uthread_impl.h"

static ucontext_t sched_ctx;



void
uthread_yield()
{
    uthread *prev_current = current_thread;
    do {
        current_thread = ll_choose_thread();
    } while (current_thread == prev_current);

    volatile int jumped = 0;
    getcontext(&prev_current->ctx);

    if (jumped) {
        //printf("[uthread_yield] jumped = true => return to func\n");
        return;
    }
    jumped = 1;

    if (setcontext(&current_thread->ctx) == -1) {
        err(EXIT_FAILURE, "swapcontext failed");
    }
}

ucontext_t *
get_uc_link()
{
    volatile int jumped = 0;
    getcontext(&sched_ctx);

    if (jumped) {
        /* uthread returned and it's context now is sched_ctx */
        /* remove thread here .... */
        uthread_yield();
    }

    jumped = 1;
    return &sched_ctx;
}
