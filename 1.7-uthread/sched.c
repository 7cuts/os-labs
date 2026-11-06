#include <assert.h>
#include <stddef.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/time.h>
#include <unistd.h>

#include "sched.h"
#include "uthread_impl.h"
#include "thread_list.h"

static jmp_buf sched_env; // The way to jump to scheduler
static uthread *current_thread;

static void
sigalarm_handler(int signo, siginfo_t *info, void *ctx)
{
    // TODO: do i really need to save signal mask?
    // TODO: check if it is *our* signal

    write(1, "sigalarm_handler worked\n", 24);
    int not_first_call = sigsetjmp(current_thread->env, 1);

    if (!not_first_call) {
        siglongjmp(sched_env, 0); // jump to scheduler
    }

    // scheduler choosed our thread to execute
}

void
do_schedule()
{
    // we jumped here from sigalarm_handler()
    uthread *thread = ll_choose_thread();

    if (thread == NULL) {
        // but we're stil in main thread and have not finished
        // the initialization. (we unblocked signals in schedule_init()
        // but we did not return from the function and signal arrived).
    }

    siglongjmp(thread->env, 0);
}

void
schedule_init()
{
    static int inited;

    if (inited) {
        return;
    }

    // Do not use SA_RESTART. It's a bad idea
    // because we swapping the context of a "thread".
    struct sigaction act = {
        .sa_sigaction = sigalarm_handler
    };
    sigaction(SIGALRM, &act, NULL);

    // Block SIGALRM
    sigset_t sigalarm;
    sigemptyset(&sigalarm);
    sigaddset(&sigalarm, SIGALRM);
    sigprocmask(SIG_BLOCK, &sigalarm, NULL);


    struct itimerval interval = {
        .it_interval = {
            .tv_sec = 1,
            .tv_usec = 0
        }
    };
    int ret = setitimer(ITIMER_REAL, &interval, NULL);
    assert(ret == 0);


    inited = 1;

    int not_first_call = sigsetjmp(sched_env, 1);

    sigprocmask(SIG_UNBLOCK, &sigalarm, NULL);

    if (!not_first_call) {
        // continue to execute main thread.
        return;
    }

    do_schedule();

}

void
sched_stop()
{
    // block SIGALARM
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);

    sigprocmask(SIG_BLOCK, &set, NULL);
}


void
sched_continue()
{
    // unblock SIGALARM
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);

    sigprocmask(SIG_UNBLOCK, &old, NULL);
}
