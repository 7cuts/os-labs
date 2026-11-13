
#include <stdio.h>
#include "test.h"

#define RED "\033[31m"
#define RESET "\033[0m"
#define GREEN "\033[32m"

bool do_test(test_result_t (*func)(FILE *f), FILE *f) {
    test_result_t res = func(f);
    printf(":: [%4s] %s", res.ok ? GREEN"OK"RESET : RED"FAIL"RESET, res.message);
    return res.ok;
}


#define TEST_DEF(name) \
    extern test_result_t name(FILE *);
#include "test_definitions.h"
#undef TEST_DEF


void
test_all(FILE *logfile)
{
#define TEST_DEF(name) \
    do_test(name, logfile);
#include "test_definitions.h"
#undef TEST_DEF
}

int
main(int argc, char *argv[])
{
    FILE *f = fopen("test.log", "w");
    if (!f) {
        fprintf(stderr, "failed to create log file \"test.log\"\n");
        return 1;
    }
    test_all(f);
    putc('\n', stdout);

    fclose(f);
}
