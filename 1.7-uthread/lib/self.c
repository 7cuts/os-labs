#include "uthread.h"
#include "uthread_impl.h"

uthread *current_thread;

uthread_t uthread_self() {
    if (!current_thread) {
        return (uthread_t)NULL;
    }
    return current_thread->self;
}
