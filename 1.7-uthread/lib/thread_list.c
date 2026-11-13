#include "uthread_impl.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uthread *head;
static uthread *tail;
static uthread main_tcb;
static int count;

void
ll_add(uthread *tcb)
{
    if (!head) {
        // first time calling it. we are in main thread.
        assert(!tail);
        assert(count == 0);

        head = &main_tcb;

        strncpy(main_tcb.name, "main", sizeof(main_tcb.name));
        main_tcb.self = (uthread_t) &main_tcb;
        tail = head;
        srand(time(NULL));
        count++;
    }

    // add new thread structure to the list of threads.
    assert(!tail->next);
    tail->next = tcb;
    tcb->prev = tail;
    tail = tcb;
    count++;
}

void
ll_remove(uthread *tcb)
{
    if (tcb == NULL) return;

    if (!tcb->next && !tcb->prev) {
        assert(0);
        // it is strange .....
        return;
    }

    if (!tcb->prev) {
        /* it is the head */
        tcb->next->prev = NULL;
        head = tcb->next;
        goto post_remove;
    }

    if (!tcb->next) {
        // it is the tai
        tcb->prev->next = NULL;
        tail = tcb->prev;
        goto post_remove;
    }

    // nor head nor tail. we're in the middle

    tcb->prev->next = tcb->next;
    tcb->next->prev = tcb->prev;

    tcb->next = NULL;
    tcb->prev = NULL;

post_remove:
    count--;
}


bool
ll_find(uthread *needle)
{
    for (uthread* p = head; p != NULL; p = p->next) {
        if (p == needle) {
           return true;
        }
    }
    return false;
}

int
ll_length()
{
    return count;
}

/* May return NULL in case all threads is exited */
uthread *
ll_choose_thread(void)
{
    int alive = 0;
    for (uthread *p = head; p; p = p->next) {
        if (!p->exited) {
            alive++;
        }
    }

    if (alive == 0) {
        return NULL;
    }

    int num = rand() % alive;

    for (uthread *p = head; p; p = p->next) {
        /* iterate only through running threads */
        if (!p->exited) {
            if (num-- == 0) {
                return p;
            }
        }
    }
    return NULL;
}

uthread *
ll_head()
{
    return head;
}
