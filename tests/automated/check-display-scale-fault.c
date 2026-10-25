/* Test-only calloc fault injector for the display-scale rollback fixture. */
#define _GNU_SOURCE
#include <stddef.h>
#include <signal.h>

extern void *__libc_calloc(size_t count, size_t size);
static volatile sig_atomic_t fail_calloc;

static void
set_failure(int signal_number)
{
    fail_calloc = signal_number == SIGUSR1;
}

__attribute__((constructor)) static void
initialize(void)
{
    signal(SIGUSR1, set_failure);
    signal(SIGUSR2, set_failure);
}

void *
calloc(size_t count, size_t size)
{
    if (fail_calloc)
        return NULL;
    return __libc_calloc(count, size);
}
