#include <err.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "config.h"
#include "tcpcube_worker.h"

void* tcpcube_worker_start(void* worker_args)
{
//  tcpcube_module_tlinit()

//    pthread_cleanup_push()
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    sigset_t signal_set;
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGINT);
    if(pthread_sigmask(SIG_BLOCK, &signal_set, NULL) != 0)
        errx(EXIT_FAILURE, "%s: %u: pthread_sigmask() failed", __FILE__, __LINE__);

    if(((struct tcpcube_worker_args*)worker_args)->tcpcube_module_start(((struct tcpcube_worker_args*)worker_args)->module, &((struct tcpcube_worker_args*)worker_args)->server_socket, ((struct tcpcube_worker_args*)worker_args)->server_socket_mutex) == -1)
        errx(EXIT_FAILURE, "%s: %u: tcpcube_module_start() failed", __FILE__, __LINE__);
//    pthread_cleanup_pop(, 1)
    return NULL;
}
