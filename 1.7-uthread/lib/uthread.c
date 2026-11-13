
#include <assert.h>
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <ucontext.h>

#include "uthread.h"
#include "uthread_impl.h"

#define CTX_SIZE (sizeof(uthread))
#define STACK_SIZE (8 * 1024 * 1024 - CTX_SIZE)
#define GUARD_PAGE_SIZE 4096
#define THREAD_MEM (STACK_SIZE + CTX_SIZE + GUARD_PAGE_SIZE)



/* Return NULL if cannot alloc */
static void *
alloc_thread()
{
    void *res = mmap(NULL, THREAD_MEM,
        PROT_READ | PROT_WRITE, MAP_STACK | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    assert(errno != EINVAL);
    if (res == MAP_FAILED) {
        return NULL;
    }

    return res;
}


static void
thread_start()
{
    current_thread->retval = current_thread->start_func(current_thread->arg);
    current_thread->exited = true;
}

int
uthread_create(uthread_t* thread, void *(*start_routine)(void *), void *arg, const char *name)
{
    if (!start_routine) {
        return -EINVAL;
    }


    if (!current_thread) {
        /* assume that ll_head() is main thread tcb */
        current_thread = ll_head();
    }

    UTHREAD_DEBUG("creating thread %s", name);

    void *region = alloc_thread();
    if (region == NULL) {
        return -EAGAIN; /* mmap failed. not enough memory? */
    }
    /* MEMORY layout:  0x0 .. [struct uthread | STACK | GUARD PAGE] .. 0xF..F */
    uthread *tcb = region;
    void *stack = region + sizeof(uthread);
    void *guard_page = stack + STACK_SIZE;

    if (mprotect(guard_page, GUARD_PAGE_SIZE, PROT_NONE) == -1) {
        fprintf(stderr, "mprotect guard page failed: %s\n", strerror(errno));
        exit(1);
    }

    tcb->arg = arg;
    tcb->start_func = start_routine;
    strncpy(tcb->name, name, sizeof(tcb->name));
    tcb->self = (uthread_t)tcb;

    getcontext(&tcb->ctx);
    tcb->ctx.uc_stack.ss_sp = stack;
    tcb->ctx.uc_stack.ss_size = STACK_SIZE;

    /* prevent thread exits after it returns. instead go to scheduler */
    tcb->ctx.uc_link = get_uc_link();
    makecontext(&tcb->ctx,
        thread_start,
        0);

    ll_add(tcb);

    *thread = (uthread_t) tcb;

    return 0;
}

int uthread_join(uthread_t thread, void **retval)
{
    uthread *tcb = (uthread *) thread;
    bool found = ll_find(tcb);

    if (!found) {
        return -EINVAL;
    }

    UTHREAD_DEBUG("joining thread %s", tcb->name);

    while (!tcb->exited) {
        uthread_yield();
    }

    if (retval)
    {
        *retval = tcb->retval;
    }

    UTHREAD_DEBUG("remove thread %s from linked list", tcb->name);
    ll_remove(tcb);
    int r = munmap(tcb, THREAD_MEM);
    assert(r == 0);
    return 0;
}
