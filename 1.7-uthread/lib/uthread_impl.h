#ifndef UTHREAD_IMPL_H
#define UTHREAD_IMPL_H

#include <stdbool.h>
#include <ucontext.h>
#include "uthread.h"

#define CTX_SIZE (sizeof(uthread))
#define STACK_SIZE (8 * 1024 * 1024 - CTX_SIZE)
#define GUARD_PAGE_SIZE 4096
#define THREAD_MEM (STACK_SIZE + CTX_SIZE + GUARD_PAGE_SIZE)


typedef struct uthread uthread;
struct uthread {
    uthread_t self;

    /* We need arg only for call a start_routine. after
     * that we can reuse this field to store retval. */
    union {
        void *arg;
        void *retval;
    };

    void * (*start_func)(void *);

    uthread *next;
    uthread *prev;
    ucontext_t ctx;

    char name[32];

    bool exited;

};



/************** from self.c **************/
extern uthread *current_thread;

/********** from thread_list.c ***********/
void ll_add(uthread *tcb);
void ll_remove(uthread *tcb);
bool ll_find(uthread *needle);
int ll_length();
uthread *ll_choose_thread();
uthread *ll_head();

/*************** from sched.c *************/
ucontext_t *get_uc_link();

// #define LOG_DEBUG /* define this enable debug prints from library */
#ifdef LOG_DEBUG
#define UTHREAD_DEBUG(fmt, ...) uthread_printf(fmt, ##__VA_ARGS__)
#else
#define UTHREAD_DEBUG(fmt, ...)
#endif

#endif
