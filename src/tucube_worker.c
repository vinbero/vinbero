#include <err.h>
#include <libgonc/gonc_cast.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "config.h"
#include "tucube_worker.h"

static void tucube_worker_pthread_cleanup_handler(void* worker_args)
{
    if(GONC_CAST(worker_args, struct tucube_worker_args*)->tucube_module_tldestroy(((struct tucube_worker_args*)worker_args)->module) == -1)
        warnx("%s: %u: tucube_module_tldestroy() failed", __FILE__, __LINE__);
    pthread_mutex_lock(GONC_CAST(worker_args, struct tucube_worker_args*)->exit_mutex);
    GONC_CAST(worker_args, struct tucube_worker_args*)->exit = true;
    pthread_cond_signal(GONC_CAST(worker_args, struct tucube_worker_args*)->exit_cond);
    pthread_mutex_unlock(GONC_CAST(worker_args, struct tucube_worker_args*)->exit_mutex);
}

void* tucube_worker_start(void* worker_args)
{
    pthread_cleanup_push(tucube_worker_pthread_cleanup_handler, worker_args);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if(((struct tucube_worker_args*)worker_args)->tucube_module_tlinit(((struct tucube_worker_args*)worker_args)->module, ((struct tucube_worker_args*)worker_args)->module_args) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_module_tlinit() failed", __FILE__, __LINE__);

    sigset_t signal_set;
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGINT);
    if(pthread_sigmask(SIG_BLOCK, &signal_set, NULL) != 0)
        errx(EXIT_FAILURE, "%s: %u: pthread_sigmask() failed", __FILE__, __LINE__);

    if(((struct tucube_worker_args*)worker_args)->tucube_module_start(((struct tucube_worker_args*)worker_args)->module, &((struct tucube_worker_args*)worker_args)->server_socket, ((struct tucube_worker_args*)worker_args)->server_socket_mutex) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_module_start() failed", __FILE__, __LINE__);

    pthread_cleanup_pop(1);

    return NULL;
}
