#include "uthread_impl.h"
#include <assert.h>
#include <stdbool.h>
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
        tail = head;

        srand(time(NULL));
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

uthread *
ll_choose_thread()
{
    int num = rand() % count;

    if (count == 0) {
        return NULL;
    }

    uthread *p = head;
    for (int i = 0; i < num; i++) {
        p = p->next;
    }
    return p;
}
