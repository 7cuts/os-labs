#include <stdio.h>
#include "uthread.h"
#include <unistd.h>

int cnt;

void *start_func(void *arg) {
    printf("i'm started\n");
    for (int i = 0; i < 100; i++) {
        cnt++;
    }
}

int main() {
    uthread_t thread;
    uthread_create(&thread, start_func, NULL);
    uthread_create(&thread, start_func, NULL);
    uthread_create(&thread, start_func, NULL);

    start_func(NULL);

    printf("cnt = %d\n", cnt);

    sleep(1);
    for (int i =0; i < 100000; i++) {}
    printf("cnt = %d\n", cnt);

    sleep(1);
    for (int i =0; i < 100000; i++) {}
    printf("cnt = %d\n", cnt);

    sleep(1);
    for (int i =0; i < 100000; i++) {}
    printf("cnt = %d\n", cnt);
    return 0;
}
