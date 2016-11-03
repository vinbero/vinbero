#include <arpa/inet.h>
#include <dlfcn.h>
#include <err.h>
#include <pthread.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/syscall.h>
#include <libgonc/gonc_list.h>
#include <libgonc/gonc_ltostr.h>
#include "config.h"
#include "tucube_Master.h"
#include "tucube_Worker.h"

static pthread_key_t tucube_Master_tlKey;

static void tucube_Master_sigIntHandler(int signal_number) {
    exit(EXIT_FAILURE);
}

static void tucube_Master_exitHandler() {
    if(syscall(SYS_gettid) == getpid()) {
        jmp_buf* jumpBuffer = pthread_getspecific(tucube_Master_tlKey);
        if(jumpBuffer != NULL)
            longjmp(*jumpBuffer, 1);
    }
}

static void tucube_Master_registerSignalHandlers() {
    struct sigaction signalAction;
    signalAction.sa_handler = tucube_Master_sigIntHandler;
    signalAction.sa_flags = SA_RESTART;
    if(sigfillset(&signalAction.sa_mask) == -1)
        err(EXIT_FAILURE, "%s, %u", __FILE__, __LINE__);
    if(sigaction(SIGINT, &signalAction, NULL) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    signalAction.sa_handler = SIG_IGN;
    signalAction.sa_flags = SA_RESTART;
    if(sigfillset(&signalAction.sa_mask) == -1)
        err(EXIT_FAILURE, "%s, %u", __FILE__, __LINE__);
    if(sigaction(SIGPIPE, &signalAction, NULL) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
}

void tucube_Master_initCore(struct tucube_Master* master, struct tucube_Worker* worker) { // init worker object
    worker = malloc(1 * sizeof(struct tucube_Worker));

    worker->exit = false;
    worker->exitMutex = malloc(1 * sizeof(pthread_mutex_t));
    pthread_mutex_init(worker->exitMutex, NULL);
    worker->exitCond = malloc(1 * sizeof(pthread_cond_t));
    pthread_cond_init(worker->exitCond, NULL);

    struct sockaddr_in serverAddressSockAddrIn;
    memset(serverAddressSockAddrIn.sin_zero, 0, 1 * sizeof(serverAddressSockAddrIn.sin_zero));
    serverAddressSockAddrIn.sin_family = AF_INET;
    inet_aton(master->address, &serverAddressSockAddrIn.sin_addr);
    serverAddressSockAddrIn.sin_port = htons(master->port);

    if((worker->serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setsockopt(worker->serverSocket, SOL_SOCKET, SO_REUSEADDR, &(const int){1}, sizeof(int)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setsockopt(worker->serverSocket, SOL_SOCKET, SO_REUSEPORT, &master->reusePort, sizeof(int)) == -1)
        warn("%s: %u", __FILE__, __LINE__);

    if(bind(worker->serverSocket, (struct sockaddr*)&serverAddressSockAddrIn, sizeof(struct sockaddr)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(listen(worker->serverSocket, master->backlog) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    worker->serverSocketMutex = malloc(1 * sizeof(pthread_mutex_t));
    pthread_mutex_init(worker->serverSocketMutex, NULL);
}

void tucube_Master_initModules(struct tucube_Master* master, struct tucube_Worker* worker) {
    if((master->dlHandle = dlopen(GONC_LIST_HEAD(master->moduleConfigList)->modulePath, RTLD_LAZY | RTLD_GLOBAL)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if((master->tucube_Module_init = dlsym(master->dlHandle, "tucube_Module_init")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_init()", __FILE__, __LINE__);

    if((worker->tucube_Module_tlInit = dlsym(master->dlHandle, "tucube_Module_tlInit")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_tlInit()", __FILE__, __LINE__);

    if((worker->tucube_Module_start = dlsym(master->dlHandle, "tucube_Module_start")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_start()", __FILE__, __LINE__);

    if((worker->tucube_Module_tlDestroy = dlsym(master->dlHandle, "tucube_Module_tlDestroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_tlDestroy()", __FILE__, __LINE__);

    if((master->tucube_Module_destroy = dlsym(master->dlHandle, "tucube_Module_destroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_destroy()", __FILE__, __LINE__);

    GONC_LIST_FOR_EACH(master->moduleConfigList, struct tucube_Module_Config, moduleConfig)
    {
        struct tucube_Module_Arg* moduleArg = malloc(1 * sizeof(struct tucube_Module_Arg));
        GONC_LIST_ELEMENT_INIT(moduleArg);
        moduleArg->name = strdup("tucube-worker-count");
        gonc_ltostr(workerCount, 10, &moduleArg->value);
        GONC_LIST_APPEND(moduleConfig, moduleArg);
    }

    master->moduleList = malloc(1 * sizeof(struct tucube_Module_List));
    GONC_LIST_INIT(master->moduleList);
    if(master->tucube_Module_init(GONC_LIST_HEAD(master->moduleConfigList), master->moduleList) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_Module_init() failed", __FILE__, __LINE__);

    worker->module = GONC_LIST_HEAD(master->moduleList);
    worker->moduleConfig = GONC_LIST_HEAD(master->moduleConfigList);
}

void tucube_Master_start(struct tucube_Master* master, struct tucube_Worker* worker) {
    tucube_Master_registerSignalHandlers();
    pthread_t* workerThreads;
    pthread_attr_t workerThreadAttr;
    jmp_buf* jumpBuffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jumpBuffer) == 0)
    {
        pthread_key_create(&tucube_Master_tlKey, NULL);
        pthread_setspecific(tucube_Master_tlKey, jumpBuffer);

        pthread_attr_init(&workerThreadAttr);
        pthread_attr_setdetachstate(&workerThreadAttr, PTHREAD_CREATE_JOINABLE);

        workerThreads = malloc(workerCount * sizeof(pthread_t));

        atexit(tucube_Master_exitHandler);

        for(size_t index = 0; index != workerCount; ++index)
        {
            if(pthread_create(workerThreads + index, &workerThreadAttr, tucube_Worker_start, worker) != 0)
                err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        }

        pthread_mutex_lock(worker->exitMutex);
        while(worker->exit != true)
        {
            pthread_cond_wait(worker->exitCond,
                 worker->exitMutex);
        }
        pthread_mutex_unlock(worker->exitMutex);

        for(size_t index = 0; index != workerCount; ++index)
        {
            pthread_cancel(workerThreads[index]);
            pthread_join(workerThreads[index], NULL);
        }
        worker->exit = true;
    }
    free(jumpBuffer);
    pthread_key_delete(tucube_Master_tlKey);
    pthread_mutex_unlock(worker->exitMutex);

    if(worker->exit == false) {
        for(size_t index = 0; index != workerCount; ++index) {
            pthread_cancel(workerThreads[index]);
            pthread_mutex_lock(worker->exitMutex);
            while(worker->exit != true) {
                pthread_cond_wait(worker->exitCond,
                     worker->exitMutex);
            }
            pthread_mutex_unlock(worker->exitMutex);
            pthread_join(workerThreads[index], NULL);
            worker->exit = false;
        }
    }

    pthread_cond_destroy(worker->exitCond);
    free(worker->exitCond);
    pthread_mutex_destroy(worker->exitMutex);
    free(worker->exitMutex);

    close(worker->serverSocket);
    pthread_mutex_destroy(worker->serverSocketMutex);
    free(worker->serverSocketMutex);
    free(worker);

    pthread_attr_destroy(&workerThreadAttr);
    free(workerThreads);

    if(master->tucube_Module_destroy(GONC_LIST_HEAD(master->moduleList)) == -1)
        warn("%s: %u", __FILE__, __LINE__);
    free(master->moduleList);

    GONC_LIST_REMOVE_FOR_EACH(master->moduleConfigList, struct tucube_Module_Config, moduleConfig) {
        GONC_LIST_REMOVE_FOR_EACH(moduleConfig, struct tucube_Module_Arg, moduleArg) {
            GONC_LIST_REMOVE(moduleConfig, moduleArg);
            free(moduleArg->name);
            free(moduleArg->value);
            free(moduleArg);
        }
        GONC_LIST_REMOVE(master->moduleConfigList, moduleConfig);
        free(moduleConfig->modulePath);
        free(moduleConfig);
    }
    free(master->moduleConfigList);

//    dlclose(master->dlHandle);
}
