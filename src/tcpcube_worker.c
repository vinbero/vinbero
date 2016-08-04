#include <err.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "config.h"
#include "tcpcube_worker.h"

static void tcpcube_worker_pthread_cleanup_handler(void* worker_args)
{
    if(((struct tcpcube_worker_args*)worker_args)->tcpcube_module_tldestroy(((struct tcpcube_worker_args*)worker_args)->module) == -1)
        warnx("%s: %u: tcpcube_module_tldestroy() failed", __FILE__, __LINE__);
}

void* tcpcube_worker_start(void* worker_args)
{
    if(((struct tcpcube_worker_args*)worker_args)->tcpcube_module_tlinit(((struct tcpcube_worker_args*)worker_args)->module) == -1)
        errx(EXIT_FAILURE, "%s: %u: tcpcube_module_tlinit() failed", __FILE__, __LINE__);
    pthread_cleanup_push(tcpcube_worker_pthread_cleanup_handler, worker_args);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    sigset_t signal_set;
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGINT);
    if(pthread_sigmask(SIG_BLOCK, &signal_set, NULL) != 0)
        errx(EXIT_FAILURE, "%s: %u: pthread_sigmask() failed", __FILE__, __LINE__);

    if(((struct tcpcube_worker_args*)worker_args)->tcpcube_module_start(((struct tcpcube_worker_args*)worker_args)->module, &((struct tcpcube_worker_args*)worker_args)->server_socket, ((struct tcpcube_worker_args*)worker_args)->server_socket_mutex) == -1)
        errx(EXIT_FAILURE, "%s: %u: tcpcube_module_start() failed", __FILE__, __LINE__);
    pthread_cleanup_pop(1);
    return NULL;
}
