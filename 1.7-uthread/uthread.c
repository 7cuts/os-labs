#include "uthread.h"
#include "setjmp.h"


typedef struct {
    void *(*start_routine)(void *arg);
    void *retval;
    jmp_buf env;
} uthread;


static jmp_buf sched_env;

static void schedule()
{
    setjmp(sched_env);

    // 1. выбрать поток, который поставить на исполнение
    // 2. завести будильник, чтобы вытеснить поток
    // 3. запустить поток

    uthread thread; // TODO: get any running thread

    longjmp(thread.env, 0);
}

void sched_yeild()
{
    // save current thread's context
    uthread self; // TODO: somehow get it
    int not_first_call = setjmp(self.env);

    if (!not_first_call) {
        longjmp(sched_env, 0); // go to scheduler
    }

    // we're back after some time
    return;
}

int uthread_create(uthread_t* thread, void *(*start_routine)(void *arg), void *arg)
{
    if (!start_routine) {
        return -EINVAL;
    }


    sched_yeild();
    return 0;
}
