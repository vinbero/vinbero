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
#include "tucube_Core.h"

static pthread_key_t tucube_Core_tlKey;

static void tucube_Core_sigIntHandler(int signal_number) {
    exit(EXIT_FAILURE);
}

static void tucube_Core_exitHandler() {
    if(syscall(SYS_gettid) == getpid()) {
        jmp_buf* jumpBuffer = pthread_getspecific(tucube_Core_tlKey);
        if(jumpBuffer != NULL)
            longjmp(*jumpBuffer, 1);
    }
}

static void tucube_Core_registerSignalHandlers() {
    struct sigaction signalAction;
    signalAction.sa_handler = tucube_Core_sigIntHandler;
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

int tucube_Core_init(struct tucube_Core* core, struct tucube_Core_Config* coreConfig, struct tucube_Module_ConfigList* moduleConfigList) {

    core->exit = false;
    core->exitMutex = malloc(1 * sizeof(pthread_mutex_t));
    pthread_mutex_init(core->exitMutex, NULL);
    core->exitCond = malloc(1 * sizeof(pthread_cond_t));
    pthread_cond_init(core->exitCond, NULL);

    struct sockaddr_in serverAddressSockAddrIn;
    memset(serverAddressSockAddrIn.sin_zero, 0, 1 * sizeof(serverAddressSockAddrIn.sin_zero));
    serverAddressSockAddrIn.sin_family = AF_INET;
    char* address = "0.0.0.0";
    if(json_object_get(coreConfig, "tucube.address") != NULL)
        address = json_string_value(json_object_get(coreConfig, "

    inet_aton(address, &serverAddressSockAddrIn.sin_addr);

    int port = 8080;
    if(json_object_get(coreConfig, "tucube.port") != NULL)
        port = json_integer_value(json_object_get(coreConfig, "tucube.port"));
    serverAddressSockAddrIn.sin_port = htons(core->port);

    if((core->serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__)

    if(setsockopt(core->serverSocket, SOL_SOCKET, SO_REUSEADDR, &(const int){1}, sizeof(int)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    int reusePort = 0;
    if(json_object_get(coreConfig, "tucube.reusePort") != NULL)
        reusePort = json_integer_value(json_object_get(coreConfig, "tucube.reusePort");
    if(setsockopt(core->serverSocket, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(int)) == -1)
        warn("%s: %u", __FILE__, __LINE__);

    if(bind(core->serverSocket, (struct sockaddr*)&serverAddressSockAddrIn, sizeof(struct sockaddr)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    int backlog = 5;
    if(json_object_get(coreConfig, "tucube.backlog") != NULL)
        backlog = json_integer_value(json_object_get(coreConfig, "tucube.backlog"));
    if(listen(core->serverSocket, backlog) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    core->serverSocketMutex = malloc(1 * sizeof(pthread_mutex_t));
    pthread_mutex_init(core->serverSocketMutex, NULL);
    return 0;
}

void tucube_Core_initModules(struct tucube_Core* core) {
    if((core->dlHandle = dlopen(GONC_LIST_HEAD(core->moduleConfigList)->modulePath, RTLD_LAZY | RTLD_GLOBAL)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if((core->tucube_Module_init = dlsym(core->dlHandle, "tucube_Module_init")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_init()", __FILE__, __LINE__);

    if((core->tucube_Module_tlInit = dlsym(core->dlHandle, "tucube_Module_tlInit")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_tlInit()", __FILE__, __LINE__);

    if((core->tucube_Module_start = dlsym(core->dlHandle, "tucube_Module_start")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_start()", __FILE__, __LINE__);

    if((core->tucube_Module_tlDestroy = dlsym(core->dlHandle, "tucube_Module_tlDestroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_tlDestroy()", __FILE__, __LINE__);

    if((core->tucube_Module_destroy = dlsym(core->dlHandle, "tucube_Module_destroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_Module_destroy()", __FILE__, __LINE__);

    GONC_LIST_FOR_EACH(core->moduleConfigList, struct tucube_Module_Config, moduleConfig)
    {
        struct tucube_Module_Arg* moduleArg = malloc(1 * sizeof(struct tucube_Module_Arg));
        GONC_LIST_ELEMENT_INIT(moduleArg);
        moduleArg->name = strdup("tucube-core-count");
        gonc_ltostr(coreCount, 10, &moduleArg->value);
        GONC_LIST_APPEND(moduleConfig, moduleArg);
    }

    core->moduleList = malloc(1 * sizeof(struct tucube_Module_List));
    GONC_LIST_INIT(core->moduleList);
    if(core->tucube_Module_init(GONC_LIST_HEAD(core->moduleConfigList), core->moduleList) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_Module_init() failed", __FILE__, __LINE__);

    core->module = GONC_LIST_HEAD(core->moduleList);
    core->moduleConfig = GONC_LIST_HEAD(core->moduleConfigList);
}

void tucube_Core_start(struct tucube_Core* core) {
    tucube_Core_registerSignalHandlers();
    pthread_t* coreThreads;
    pthread_attr_t coreThreadAttr;
    jmp_buf* jumpBuffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jumpBuffer) == 0)
    {
        pthread_key_create(&tucube_Core_tlKey, NULL);
        pthread_setspecific(tucube_Core_tlKey, jumpBuffer);

        pthread_attr_init(&coreThreadAttr);
        pthread_attr_setdetachstate(&coreThreadAttr, PTHREAD_CREATE_JOINABLE);

        coreThreads = malloc(coreCount * sizeof(pthread_t));

        atexit(tucube_Core_exitHandler);

        for(size_t index = 0; index != coreCount; ++index)
        {
            if(pthread_create(coreThreads + index, &coreThreadAttr, tucube_Worker_start, core) != 0)
                err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        }

        pthread_mutex_lock(core->exitMutex);
        while(core->exit != true)
        {
            pthread_cond_wait(core->exitCond,
                 core->exitMutex);
        }
        pthread_mutex_unlock(core->exitMutex);

        for(size_t index = 0; index != coreCount; ++index)
        {
            pthread_cancel(coreThreads[index]);
            pthread_join(coreThreads[index], NULL);
        }
        core->exit = true;
    }
    free(jumpBuffer);
    pthread_key_delete(tucube_Core_tlKey);
    pthread_mutex_unlock(core->exitMutex);

    if(core->exit == false) {
        for(size_t index = 0; index != coreCount; ++index) {
            pthread_cancel(coreThreads[index]);
            pthread_mutex_lock(core->exitMutex);
            while(core->exit != true) {
                pthread_cond_wait(core->exitCond,
                     core->exitMutex);
            }
            pthread_mutex_unlock(core->exitMutex);
            pthread_join(coreThreads[index], NULL);
            core->exit = false;
        }
    }

    pthread_cond_destroy(core->exitCond);
    free(core->exitCond);
    pthread_mutex_destroy(core->exitMutex);
    free(core->exitMutex);

    close(core->serverSocket);
    pthread_mutex_destroy(core->serverSocketMutex);
    free(core->serverSocketMutex);
    free(core);

    pthread_attr_destroy(&coreThreadAttr);
    free(coreThreads);

    if(core->tucube_Module_destroy(GONC_LIST_HEAD(core->moduleList)) == -1)
        warn("%s: %u", __FILE__, __LINE__);
    free(core->moduleList);

//    dlclose(core->dlHandle);
}

static void tucube_Core_pthreadCleanupHandler(void* core) {
    if(GONC_CAST(core, struct tucube_Core*)->tucube_Module_tlDestroy(((struct tucube_Core*)core)->module) == -1)
        warnx("%s: %u: tucube_Module_tlDestroy() failed", __FILE__, __LINE__);
    pthread_mutex_lock(GONC_CAST(core, struct tucube_Core*)->exitMutex);
    GONC_CAST(core, struct tucube_Core*)->exit = true;
    pthread_cond_signal(GONC_CAST(core, struct tucube_Core*)->exitCond);
    pthread_mutex_unlock(GONC_CAST(core, struct tucube_Core*)->exitMutex);
}

void* tucube_Core_startWorker(void* core) {
    pthread_cleanup_push(tucube_Core_pthreadCleanupHandler, core);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if(((struct tucube_Core*)core)->tucube_Module_tlInit(((struct tucube_Core*)core)->module, ((struct tucube_Core*)core)->moduleArgs) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_Module_tlInit() failed", __FILE__, __LINE__);

    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGINT);
    if(pthread_sigmask(SIG_BLOCK, &signalSet, NULL) != 0)
        errx(EXIT_FAILURE, "%s: %u: pthread_sigmask() failed", __FILE__, __LINE__);

    if(((struct tucube_Core*)core)->tucube_Module_start(((struct tucube_Core*)core)->module, &((struct tucube_Core*)core)->serverSocket, ((struct tucube_Core*)core)->serverSocketMutex) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_Module_start() failed", __FILE__, __LINE__);

    pthread_cleanup_pop(1);

    return NULL;
}

