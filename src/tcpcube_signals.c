#include <err.h>
#include <stdlib.h>
#include "tcpcube_signals.h"

void tcpcube_sigint_handler(int signal_number)
{
    warnx("tcpcube_sigint_handler");
    exit(EXIT_FAILURE);
}
