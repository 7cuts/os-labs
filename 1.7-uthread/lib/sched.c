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

char sched_stack[8 * 1024 * 1024];

void
uthread_yield()
{
    if (!current_thread) {
        /* assume that ll_head() is main thread tcb */
        current_thread = ll_head();
    }

    uthread *prev_current = current_thread;
    uthread *next = ll_choose_thread();
    if (next) {
        current_thread = next;
    } else {
        return;
    }

    volatile int jumped = 0;
    getcontext(&prev_current->ctx);

    if (jumped) {
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
    static ucontext_t sched_ctx;
    static bool inited;

    if (!inited) {
        getcontext(&sched_ctx);
        sched_ctx.uc_stack.ss_sp = sched_stack;
        sched_ctx.uc_stack.ss_size = sizeof(sched_stack);
        sched_ctx.uc_link = NULL;
        makecontext(&sched_ctx, uthread_yield, 0);
        inited = true;
    }

    return &sched_ctx;
}
