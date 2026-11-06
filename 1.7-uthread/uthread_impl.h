#ifndef UTHREAD_IMPL_H
#define UTHREAD_IMPL_H

#include <setjmp.h>

typedef struct uthread uthread;
struct uthread {
    // We need arg only for call a start_routine. after
    // that we can reuse this field to store retval.
    union {
        void *arg;
        void *retval;
    };

    jmp_buf env;   // Enviroment of a thread before went to scheduler

    uthread *next;
    uthread *prev;

};

#endif
