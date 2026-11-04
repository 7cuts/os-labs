#include "uthread.h"
#include "setjmp.h"
#include <asm-generic/errno-base.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>


static jmp_buf sched_env; // The way to jump to scheduler

typedef struct {
    // We need arg only for call a start_routine. after
    // that we can reuse this field to store retval.
    union {
        void *arg;
        void *retval;
    };

    jmp_buf env;   // Enviroment of a thread before went to scheduler

} uthread;


#define CTX_SIZE (sizeof(uthread))
#define STACK_SIZE (8 * 1024 * 1024 - CTX_SIZE)
#define GUARD_PAGE_SIZE 4096
#define THREAD_MEM (STACK_SIZE + CTX_SIZE + GUARD_PAGE_SIZE)

// Return NULL if cannot alloc
static void *
alloc_thread() {
    void *res = mmap(NULL, THREAD_MEM,
        PROT_READ | PROT_WRITE, MAP_STACK | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    assert(errno != -EINVAL);

    if (res == MAP_FAILED) {
        return NULL;
    }
}


int uthread_create(uthread_t thread, void *(*start_routine)(void *), void *arg)
{
    // block all signals to not being interrupted while creating thread
    if (!start_routine) {
        return -EINVAL;
    }

    void *region = alloc_thread();
    if (region == NULL) {
        return -EAGAIN; // mmap failed. not enough memory ?
    }
    // do not forget to unlock them.

    // Memory layout:  0x0 .. [struct uthread | STACK | GUARD PAGE] .. 0xF..F

    uthread *tcb = region;
    void *stack = region + sizeof(uthread);
    void *guard_page = stack + STACK_SIZE;

    if (mprotect(guard_page, GUARD_PAGE_SIZE, PROT_NONE) == -1) {
        fprintf(stderr, "mprotect guard page failed: %s\n", strerror(errno));
    }


}
