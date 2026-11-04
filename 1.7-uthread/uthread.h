#ifndef UTHREAD_H
#define UTHREAD_H

#include <stdint.h>

typedef uintptr_t uthread_t;

#define UTHREAD_CANCELED ((void*)-1)

int uthread_create(uthread_t thread, void *(*start_routine)(void *), void *arg);
int uthread_join(uthread_t thread, void **retval);
int uthread_cancel(uthread_t thread);

#endif
