#include "uthread_impl.h"
#include <stdarg.h>
#include <stdio.h>

static void uthread_logv(FILE *file, bool use_color, const char *format, va_list ap)
{
    uthread *self = (uthread *)uthread_self();

    if (!file)
        file = stderr;

    /* если self ещё не инициализирован — считаем, что это main */
    const char *name = (self && self->name) ? self->name : "main";

    if (use_color)
        fprintf(file, "[\033[33m%s\033[0m] ", name);
    else
        fprintf(file, "[%s] ", name);

    vfprintf(file, format, ap);
    fputc('\n', file);
}

void uthread_log(FILE *file, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    uthread_logv(file, false, format, ap);
    va_end(ap);
}

void uthread_printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    uthread_logv(stdout, true, format, ap);
    va_end(ap);
}
