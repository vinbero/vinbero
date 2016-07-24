#include <stdlib.h>
#include "tcpcube_signals.h"

void tcpcube_sigint_handler(int signal_number)
{
    exit(EXIT_FAILURE);
}
