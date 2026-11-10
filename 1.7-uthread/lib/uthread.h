#ifndef UTHREAD_H
#define UTHREAD_H

#include <stdint.h>
#include <stdio.h>

typedef uintptr_t uthread_t;

#define UTHREAD_CANCELED ((void*)-1)

/** Returns -EAGAIN if cannot alloc memory for thread */
int uthread_create(uthread_t *thread, void *(*start_routine)(void *), void *arg, const char *name);

/** Returns -EINVAL if thread is not valid ID */
int uthread_join(uthread_t thread, void **retval);

uthread_t uthread_self();


void uthread_log(FILE *file, const char *format, ...);
void uthread_printf(const char *format, ...);

void uthread_yield();
#endif
