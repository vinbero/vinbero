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
#include <unistd.h>
#include <libgenc/genc_cast.h>
#include <libgenc/genc_ArrayList.h>
#include "tucube_Core.h"


static pthread_key_t tucube_Core_tlKey;

static void tucube_Core_sigIntHandler(int signal_number) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    exit(EXIT_FAILURE);
}

static void tucube_Core_exitHandler() {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    if(syscall(SYS_gettid) == getpid()) {
        jmp_buf* jumpBuffer = pthread_getspecific(tucube_Core_tlKey);
        if(jumpBuffer != NULL)
            longjmp(*jumpBuffer, 1);
    }
}

static void tucube_Core_registerSignalHandlers() {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
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

static void tucube_Core_pthreadCleanupHandler(void* args) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_Module* module = args;
    struct tucube_Core* localModule = module->localModule.pointer;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if(childModule->tucube_IModule_tlDestroy(childModule) == -1)
            warnx("%s: %u: tucube_Module_tlDestroy() failed", __FILE__, __LINE__);
    }
    pthread_mutex_lock(localModule->exitMutex);
    localModule->exit = true;
    pthread_cond_signal(localModule->exitCond);
    pthread_mutex_unlock(localModule->exitMutex);
}

static void* tucube_Core_startWorker(void* args) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_Module* module = ((void**)args)[0];
    struct tucube_Core* localModule = module->localModule.pointer;
    struct tucube_Config* config = ((void**)args)[1];
    pthread_cleanup_push(tucube_Core_pthreadCleanupHandler, module);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if(childModule->tucube_IModule_tlInit(childModule, config, (void*[]){NULL}) == -1)
            errx(EXIT_FAILURE, "%s: %u: tucube_Module_tlInit() failed", __FILE__, __LINE__);
    }
    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGINT);
    if(pthread_sigmask(SIG_BLOCK, &signalSet, NULL) != 0)
        errx(EXIT_FAILURE, "%s: %u: pthread_sigmask() failed", __FILE__, __LINE__);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct tucube_Core_Interface* moduleInterface = childModule->interface;
        if((moduleInterface->tucube_ITlService_call = dlsym(childModule->dlHandle, "tucube_ITlService_call")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_ITlService_call()", __FILE__, __LINE__);
        if(moduleInterface->tucube_ITlService_call(childModule, (void*[]){&localModule->serverSocket, NULL}) == -1)
            errx(EXIT_FAILURE, "%s: %u: tucube_ITlService_call() failed", __FILE__, __LINE__);
    }
    pthread_cleanup_pop(1);
    return NULL;
}

static int tucube_Core_initLocalModule(struct tucube_Module* module, struct tucube_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_Core* localModule = module->localModule.pointer;
    TUCUBE_CONFIG_GET(config, module->name, "tucube.protocol", string, &localModule->protocol, "TCP");
    TUCUBE_CONFIG_GET(config, module->name, "tucube.address", string, &localModule->address, "0.0.0.0");
    TUCUBE_CONFIG_GET(config, module->name, "tucube.port", integer, &localModule->port, 8080);
    TUCUBE_CONFIG_GET(config, module->name, "tucube.reusePort", integer, &localModule->reusePort, 0);
    TUCUBE_CONFIG_GET(config, module->name, "tucube.backlog", integer, &localModule->backlog, 1024);
    TUCUBE_CONFIG_GET(config, module->name, "tucube.workerCount", integer, &localModule->workerCount, 4);
    TUCUBE_CONFIG_GET(config, module->name, "tucube.setUid", integer, &localModule->setUid, geteuid());
    TUCUBE_CONFIG_GET(config, module->name, "tucube.setGid", integer, &localModule->setGid, getegid());

/*  DO THIS IN THREADING MODULE
    localModule->exit = false;
    localModule->exitMutex = malloc(1 * sizeof(pthread_mutex_t));
    pthread_mutex_init(localModule->exitMutex, NULL);
    localModule->exitCond = malloc(1 * sizeof(pthread_cond_t));
    pthread_cond_init(localModule->exitCond, NULL);
*/

/*  DO THIS IN SOCKET MODULE
    struct sockaddr_in serverAddressSockAddrIn;
    memset(serverAddressSockAddrIn.sin_zero, 0, 1 * sizeof(serverAddressSockAddrIn.sin_zero));
    serverAddressSockAddrIn.sin_family = AF_INET; 

    inet_aton(localModule->address, &serverAddressSockAddrIn.sin_addr);
    serverAddressSockAddrIn.sin_port = htons(localModule->port);

    if(strcmp(localModule->protocol, "TCP") == 0) {
        if((localModule->serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        if(setsockopt(localModule->serverSocket, SOL_SOCKET, SO_REUSEADDR, &(const int){1}, sizeof(int)) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        if(setsockopt(localModule->serverSocket, SOL_SOCKET, SO_REUSEPORT, &localModule->reusePort, sizeof(int)) == -1)
            warn("%s: %u", __FILE__, __LINE__);
        if(bind(localModule->serverSocket, (struct sockaddr*)&serverAddressSockAddrIn, sizeof(struct sockaddr)) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        if(listen(localModule->serverSocket, localModule->backlog) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    } else if(strcmp(localModule->protocol, "UDP") == 0) {
        if((localModule->serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        if(bind(localModule->serverSocket, (struct sockaddr*)&serverAddressSockAddrIn, sizeof(struct sockaddr)) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    } else
        errx(EXIT_FAILURE, "%s: %u: Unknown protocol %s", __FILE__, __LINE__, localModule->protocol);
*/
    return 0;
}

static int tucube_Core_preInitChildModules(struct tucube_Module* module, struct tucube_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_Module_Names childModuleNames;
    GENC_ARRAY_LIST_INIT(&childModuleNames);
    TUCUBE_CONFIG_GET_CHILD_MODULE_NAMES(config, module->name, &childModuleNames);
    size_t childModuleCount = GENC_ARRAY_LIST_SIZE(&childModuleNames);
    GENC_TREE_NODE_INIT_CHILDREN(module, childModuleCount);
    GENC_ARRAY_LIST_FOR_EACH(&childModuleNames, index) {
        GENC_TREE_NODE_ADD_EMPTY_CHILD(module);
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        GENC_TREE_NODE_INIT(childModule);
        GENC_TREE_NODE_SET_PARENT(childModule, module);
        childModule->name = GENC_ARRAY_LIST_GET(&childModuleNames, index);
        const char* childModulePath = NULL;
        TUCUBE_CONFIG_GET_MODULE_PATH(config, childModule->name, &childModulePath);
        if((childModule->dlHandle = dlopen(childModulePath, RTLD_LAZY | RTLD_GLOBAL)) == NULL)
            errx(EXIT_FAILURE, "%s: %u: dlopen() failed, possible causes are:\n1. Unable to find next module\n2. The next module didn't linked required shared libraries properly", __FILE__, __LINE__);
        childModule->interface = malloc(1 * sizeof(struct tucube_Core_Interface)); // free() needed
        if((childModule->tucube_IModule_init = dlsym(childModule->dlHandle, "tucube_IModule_init")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_IModule_init()", __FILE__, __LINE__);
        if((childModule->tucube_IModule_tlInit = dlsym(childModule->dlHandle, "tucube_IModule_tlInit")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_IModule_tlInit()", __FILE__, __LINE__);
        if((childModule->tucube_IModule_tlDestroy = dlsym(childModule->dlHandle, "tucube_IModule_tlDestroy")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_IModule_tlDestroy()", __FILE__, __LINE__);
        if((childModule->tucube_IModule_destroy = dlsym(childModule->dlHandle, "tucube_IModule_destroy")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_IModule_destroy()", __FILE__, __LINE__);

        tucube_Core_preInitChildModules(childModule, config);
    }
    GENC_ARRAY_LIST_FREE(&childModuleNames);
    return 0;
}

static int tucube_Core_initChildModules(struct tucube_Module* module, struct tucube_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        childModule->tucube_IModule_init(childModule, config, (void*[]){NULL});
        tucube_Core_initChildModules(childModule, config);
    }
    return 0;
}

static int tucube_Core_destroyChildModules(struct tucube_Module* module) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if(childModule->tucube_IModule_destroy(childModule) == -1)
           warn("%s: %u", __FILE__, __LINE__);
        tucube_Core_destroyChildModules(childModule);
    }
    return 0;
}

static int tucube_Core_init(struct tucube_Module* module, struct tucube_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_Core* localModule = module->localModule.pointer;
    tucube_Core_initLocalModule(module, config);
    tucube_Core_preInitChildModules(module, config);
    tucube_Core_initChildModules(module, config);
    if(setgid(localModule->setGid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    if(setuid(localModule->setUid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    return 0;
}

int tucube_Core_start(struct tucube_Module* module, struct tucube_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_Core* localModule = module->localModule.pointer;
    tucube_Core_init(module, config);
/*  DO THIS IN THREADING MODULE
    tucube_Core_registerSignalHandlers();
    pthread_t* workerThreads;
    pthread_attr_t coreThreadAttr;
    jmp_buf* jumpBuffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jumpBuffer) == 0) {
        pthread_key_create(&tucube_Core_tlKey, NULL);
        pthread_setspecific(tucube_Core_tlKey, jumpBuffer);

        pthread_attr_init(&coreThreadAttr);
        pthread_attr_setdetachstate(&coreThreadAttr, PTHREAD_CREATE_JOINABLE);

        workerThreads = malloc(localModule->workerCount * sizeof(pthread_t));

        atexit(tucube_Core_exitHandler);

        for(size_t index = 0; index != localModule->workerCount; ++index) {
           if(pthread_create(workerThreads + index, &coreThreadAttr, tucube_Core_startWorker, (void*[]){module, config}) != 0)
                err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        }

        pthread_mutex_lock(localModule->exitMutex);
        while(localModule->exit != true) {
            pthread_cond_wait(localModule->exitCond,
                 localModule->exitMutex);
        }
        pthread_mutex_unlock(localModule->exitMutex);

        for(size_t index = 0; index != localModule->workerCount; ++index) {
            pthread_cancel(workerThreads[index]);
            pthread_join(workerThreads[index], NULL);
        }
        localModule->exit = true;
    }
    free(jumpBuffer);
    pthread_key_delete(tucube_Core_tlKey);
    pthread_mutex_unlock(localModule->exitMutex);

    if(localModule->exit == false) {
        for(size_t index = 0; index != localModule->workerCount; ++index) {
            pthread_cancel(workerThreads[index]);
            pthread_mutex_lock(localModule->exitMutex);
            while(localModule->exit != true)
                pthread_cond_wait(localModule->exitCond, localModule->exitMutex);
            pthread_mutex_unlock(localModule->exitMutex);
            pthread_join(workerThreads[index], NULL);
            localModule->exit = false;
        }
    }

    pthread_cond_destroy(localModule->exitCond);
    free(localModule->exitCond);
    pthread_mutex_destroy(localModule->exitMutex);
    free(localModule->exitMutex);
    close(localModule->serverSocket);
    pthread_attr_destroy(&coreThreadAttr);
    free(workerThreads);
*/
//    dlclose(localModule->dlHandle);
    return 0;
}
