#include <err.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include "tcpcube_signals.h"

void tcpcube_sigint_handler(int signal_number)
{
    warnx("tcpcube_sigint_handler(), tid: %d", syscall(SYS_gettid));
    exit(EXIT_FAILURE);
}
