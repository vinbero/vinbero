#include <err.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "config.h"
#include "tcpcube_worker.h"

void* tcpcube_worker_start(void* worker_args)
{
    if(((struct tcpcube_worker_args*)worker_args)->tcpcube_module_start(((struct tcpcube_worker_args*)worker_args)->module, &((struct tcpcube_worker_args*)worker_args)->server_socket, ((struct tcpcube_worker_args*)worker_args)->server_socket_mutex) == -1)
        errx(EXIT_FAILURE, "%s: %u: tcpcube_module_start() failed", __FILE__, __LINE__);
    return NULL;
}
