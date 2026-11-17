#ifndef TEST_RESULT_H
#define TEST_RESULT_H

#include <stdbool.h>
#include <stdio.h>

#include "../lib/uthread.h"

typedef struct test_result_t {
    bool ok;
    char message[256];
} test_result_t;


#define FAIL(FORMAT, ...) \
    test_result_t __res;  \
    __res.ok = false;     \
    snprintf(__res.message, sizeof(__res.message),  \
        "%s: "FORMAT"\n", __FUNCTION__, __VA_ARGS__);                   \
    return __res

#define OK(FORMAT, ...) \
    test_result_t __ok_res;  \
    __ok_res.ok = true;     \
    snprintf(__ok_res.message, sizeof(__ok_res.message),  \
        "%s: "FORMAT"\n", __FUNCTION__, ##__VA_ARGS__);                   \
    return __ok_res


#define TEST(func_name) test_result_t func_name(FILE *__test_log_file)


#define LOG(format, ...) uthread_log(__test_log_file, format, ## __VA_ARGS__)



#endif
