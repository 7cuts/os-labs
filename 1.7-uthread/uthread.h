#ifndef UTHREAD_H
#define UTHREAD_H

#include <stdint.h>
#include <asm-generic/errno-base.h>

typedef uintptr_t uthread_t;


/**
 * Errors:
 *  - `-EINVAL` - if `start_routine` is null
 */
int uthread_create(uthread_t thread, void *(start_routine) (void *arg), void *arg);


#endif
