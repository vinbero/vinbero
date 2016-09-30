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

void tucube_Master_initCore(struct tucube_Master_Args* masterArgs) {
    masterArgs->workerArgs = malloc(1 * sizeof(struct tucube_Worker_Args));

    masterArgs->workerArgs->exit = false;
    masterArgs->workerArgs->exitMutex = malloc(1 * sizeof(pthread_mutex_t));
    pthread_mutex_init(masterArgs->workerArgs->exitMutex, NULL);
    masterArgs->workerArgs->exitCond = malloc(1 * sizeof(pthread_cond_t));
    pthread_cond_init(masterArgs->workerArgs->exitCond, NULL);

    struct sockaddr_in serverAddressSockAddrIn;
    memset(serverAddressSockAddrIn.sin_zero, 0, 1 * sizeof(serverAddressSockAddrIn.sin_zero));
    serverAddressSockAddrIn.sin_family = AF_INET;
    inet_aton(masterArgs->address, &serverAddressSockAddrIn.sin_addr);
    serverAddressSockAddrIn.sin_port = htons(masterArgs->port);

    if((masterArgs->workerArgs->serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setsockopt(masterArgs->workerArgs->serverSocket, SOL_SOCKET, SO_REUSEADDR, &(const int){1}, sizeof(int)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setsockopt(masterArgs->workerArgs->serverSocket, SOL_SOCKET, SO_REUSEPORT, &masterArgs->reusePort, sizeof(int)) == -1)
        warn("%s: %u", __FILE__, __LINE__);

    if(bind(masterArgs->workerArgs->serverSocket, (struct sockaddr*)&serverAddressSockAddrIn, sizeof(struct sockaddr)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(listen(masterArgs->workerArgs->serverSocket, masterArgs->backlog) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    masterArgs->workerArgs->serverSocketMutex = malloc(1 * sizeof(pthread_mutex_t));
    pthread_mutex_init(masterArgs->workerArgs->serverSocketMutex, NULL);
}

void tucube_Master_initModules(struct tucube_Master_Args* masterArgs) {
    if((masterArgs->dlHandle = dlopen(GONC_LIST_HEAD(masterArgs->moduleArgsList)->modulePath, RTLD_LAZY | RTLD_GLOBAL)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if((masterArgs->tucube_Module_init = dlsym(masterArgs->dlHandle, "tucube_Module_init")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_init()", __FILE__, __LINE__);

    if((masterArgs->workerArgs->tucube_Module_tlInit = dlsym(masterArgs->dlHandle, "tucube_Module_tlInit")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_tlInit()", __FILE__, __LINE__);

    if((masterArgs->workerArgs->tucube_Module_start = dlsym(masterArgs->dlHandle, "tucube_Module_start")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_start()", __FILE__, __LINE__);

    if((masterArgs->workerArgs->tucube_Module_tlDestroy = dlsym(masterArgs->dlHandle, "tucube_Module_tlDestroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_tlDestroy()", __FILE__, __LINE__);

    if((masterArgs->tucube_Module_destroy = dlsym(masterArgs->dlHandle, "tucube_Module_destroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_destroy()", __FILE__, __LINE__);

    GONC_LIST_FOR_EACH(masterArgs->moduleArgsList, struct tucube_Module_Args, moduleArgs)
    {
        struct tucube_Module_Arg* moduleArg = malloc(1 * sizeof(struct tucube_Module_Arg));
        GONC_LIST_ELEMENT_INIT(moduleArg);
        moduleArg->name = strdup("tucube-worker-count");
        gonc_ltostr(masterArgs->workerCount, 10, &moduleArg->value);
        GONC_LIST_APPEND(moduleArgs, moduleArg);
    }

    masterArgs->moduleList = malloc(1 * sizeof(struct tucube_Module_List));
    GONC_LIST_INIT(masterArgs->moduleList);
    if(masterArgs->tucube_Module_init(GONC_LIST_HEAD(masterArgs->moduleArgsList), masterArgs->moduleList) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_Module_init() failed", __FILE__, __LINE__);

    masterArgs->workerArgs->module = GONC_LIST_HEAD(masterArgs->moduleList);
    masterArgs->workerArgs->moduleArgs = GONC_LIST_HEAD(masterArgs->moduleArgsList);
}

void tucube_Master_start(struct tucube_Master_Args* masterArgs) {
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

        workerThreads = malloc(masterArgs->workerCount * sizeof(pthread_t));

        atexit(tucube_Master_exitHandler);

        for(size_t index = 0; index != masterArgs->workerCount; ++index)
        {
            if(pthread_create(workerThreads + index, &workerThreadAttr, tucube_Worker_start, masterArgs->workerArgs) != 0)
                err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        }

        pthread_mutex_lock(masterArgs->workerArgs->exitMutex);
        while(masterArgs->workerArgs->exit != true)
        {
            pthread_cond_wait(masterArgs->workerArgs->exitCond,
                 masterArgs->workerArgs->exitMutex);
        }
        pthread_mutex_unlock(masterArgs->workerArgs->exitMutex);

        for(size_t index = 0; index != masterArgs->workerCount; ++index)
        {
            pthread_cancel(workerThreads[index]);
            pthread_join(workerThreads[index], NULL);
        }
        masterArgs->workerArgs->exit = true;
    }
    free(jumpBuffer);
    pthread_key_delete(tucube_Master_tlKey);
    pthread_mutex_unlock(masterArgs->workerArgs->exitMutex);

    if(masterArgs->workerArgs->exit == false) {
        for(size_t index = 0; index != masterArgs->workerCount; ++index) {
            pthread_cancel(workerThreads[index]);
            pthread_mutex_lock(masterArgs->workerArgs->exitMutex);
            while(masterArgs->workerArgs->exit != true) {
                pthread_cond_wait(masterArgs->workerArgs->exitCond,
                     masterArgs->workerArgs->exitMutex);
            }
            pthread_mutex_unlock(masterArgs->workerArgs->exitMutex);
            pthread_join(workerThreads[index], NULL);
            masterArgs->workerArgs->exit = false;
        }
    }

    pthread_cond_destroy(masterArgs->workerArgs->exitCond);
    free(masterArgs->workerArgs->exitCond);
    pthread_mutex_destroy(masterArgs->workerArgs->exitMutex);
    free(masterArgs->workerArgs->exitMutex);

    close(masterArgs->workerArgs->serverSocket);
    pthread_mutex_destroy(masterArgs->workerArgs->serverSocketMutex);
    free(masterArgs->workerArgs->serverSocketMutex);
    free(masterArgs->workerArgs);

    pthread_attr_destroy(&workerThreadAttr);
    free(workerThreads);

    if(masterArgs->tucube_Module_destroy(GONC_LIST_HEAD(masterArgs->moduleList)) == -1)
        warn("%s: %u", __FILE__, __LINE__);
    free(masterArgs->moduleList);

    GONC_LIST_REMOVE_FOR_EACH(masterArgs->moduleArgsList, struct tucube_Module_Args, moduleArgs) {
        GONC_LIST_REMOVE_FOR_EACH(moduleArgs, struct tucube_Module_Arg, moduleArg) {
            GONC_LIST_REMOVE(moduleArgs, moduleArg);
            free(moduleArg->name);
            free(moduleArg->value);
            free(moduleArg);
        }
        GONC_LIST_REMOVE(masterArgs->moduleArgsList, moduleArgs);
        free(moduleArgs->modulePath);
        free(moduleArgs);
    }
    free(masterArgs->moduleArgsList);

//    dlclose(masterArgs->dlHandle);
}
