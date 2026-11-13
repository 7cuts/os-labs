
#include <stdio.h>
#include "test.h"

extern test_result_t test_basic(FILE *f);
extern test_result_t test_arg(FILE *f);

#define RED "\033[31m"
#define RESET "\033[0m"
#define GREEN "\033[32m"

bool do_test(test_result_t (*func)(FILE *f), FILE *f) {
    test_result_t res = func(f);
    printf("[%4s] %s", res.ok ? GREEN"OK"RESET : RED"FAIL"RESET, res.message);
    return res.ok;
}

void
test_all(FILE *logfile)
{

    do_test(test_basic, logfile);
    do_test(test_arg, logfile);
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
