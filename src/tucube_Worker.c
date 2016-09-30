#include <err.h>
#include <libgonc/gonc_cast.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "config.h"
#include "tucube_Worker.h"

static void tucube_Worker_pthreadCleanupHandler(void* workerArgs) {
    if(GONC_CAST(workerArgs, struct tucube_Worker_Args*)->tucube_Module_tlDestroy(((struct tucube_Worker_Args*)workerArgs)->module) == -1)
        warnx("%s: %u: tucube_Module_tlDestroy() failed", __FILE__, __LINE__);
    pthread_mutex_lock(GONC_CAST(workerArgs, struct tucube_Worker_Args*)->exitMutex);
    GONC_CAST(workerArgs, struct tucube_Worker_Args*)->exit = true;
    pthread_cond_signal(GONC_CAST(workerArgs, struct tucube_Worker_Args*)->exitCond);
    pthread_mutex_unlock(GONC_CAST(workerArgs, struct tucube_Worker_Args*)->exitMutex);
}

void* tucube_Worker_start(void* workerArgs) {
    pthread_cleanup_push(tucube_Worker_pthreadCleanupHandler, workerArgs);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if(((struct tucube_Worker_Args*)workerArgs)->tucube_Module_tlInit(((struct tucube_Worker_Args*)workerArgs)->module, ((struct tucube_Worker_Args*)workerArgs)->moduleArgs) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_Module_tlInit() failed", __FILE__, __LINE__);

    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGINT);
    if(pthread_sigmask(SIG_BLOCK, &signalSet, NULL) != 0)
        errx(EXIT_FAILURE, "%s: %u: pthread_sigmask() failed", __FILE__, __LINE__);

    if(((struct tucube_Worker_Args*)workerArgs)->tucube_Module_start(((struct tucube_Worker_Args*)workerArgs)->module, &((struct tucube_Worker_Args*)workerArgs)->serverSocket, ((struct tucube_Worker_Args*)workerArgs)->serverSocketMutex) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_Module_start() failed", __FILE__, __LINE__);

    pthread_cleanup_pop(1);

    return NULL;
}
