#include <asm-generic/errno-base.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "sched.h"
#include "uthread.h"
#include "uthread_impl.h"
#include "thread_list.h"


#define CTX_SIZE (sizeof(uthread))
#define STACK_SIZE (8 * 1024 * 1024 - CTX_SIZE)
#define GUARD_PAGE_SIZE 4096
#define THREAD_MEM (STACK_SIZE + CTX_SIZE + GUARD_PAGE_SIZE)




// Return NULL if cannot alloc
static void *
alloc_thread()
{
    void *res = mmap(NULL, THREAD_MEM,
        PROT_READ | PROT_WRITE, MAP_STACK | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    assert(errno != -EINVAL);
    if (res == MAP_FAILED) {
        return NULL;
    }

    return res;
}


int
uthread_create(uthread_t* thread, void *(*start_routine)(void *), void *arg)
{
    schedule_init();

    // block all signals to not being interrupted while creating thread. do not forget to unlock them !!!
    sched_stop();
    {
        if (!start_routine) {
            return -EINVAL;
        }

        void *region = alloc_thread();
        if (region == NULL) {
            return -EAGAIN; // mmap failed. not enough memory ?
        }
        // MEMORY layout:  0x0 .. [struct uthread | STACK | GUARD PAGE] .. 0xF..F
        uthread *tcb = region;
        void *stack = region + sizeof(uthread);
        void *guard_page = stack + STACK_SIZE;

        if (mprotect(guard_page, GUARD_PAGE_SIZE, PROT_NONE) == -1) {
            fprintf(stderr, "mprotect guard page failed: %s\n", strerror(errno));
            exit(1);
        }

        ll_add(tcb);
        *thread = (uthread_t) tcb;
    }
    sched_continue();

    return 0;
}
