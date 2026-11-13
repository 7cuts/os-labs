#include "uthread.h"
#include "uthread_impl.h"
#include <asm-generic/errno-base.h>
#include <assert.h>
#include <sys/mman.h>


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
