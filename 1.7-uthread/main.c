#include <stdio.h>
#include "uthread.h"
#include "uthread_impl.h"
#include <unistd.h>

int cnt;

void *start_func(void *arg) {
    for (int i = 0; i < 10; i++) {
        //printf("i = %d\n", i);
        cnt++;
        uthread_yield();
        //printf("cnt = %d\n", cnt);
    }

    return NULL;
}

int main() {
    printf("my pid = %d\n", getpid());
    uthread_t thread;
    uthread_create(&thread, start_func, NULL);


    for (int i = 0; i < 10; i++) {
        uthread_yield();
    }
    printf("cnt = %d\n", cnt);

}
