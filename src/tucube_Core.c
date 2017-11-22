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

static void tucube_Core_pthreadCleanupHandler(void* args) {
    struct tucube_Core* core = args;
    GENC_ARRAY_LIST_FOR_EACH(&coreModule->childModules, index) {
        struct tucube_Core_FunctionPointers* functionPointers = &GENC_ARRAY_LIST_GET(&coreModule->functionPointersList, index);
        struct tucube_Module* childModule = GENC_ARRAY_LIST_GET(&coreModule->childModules, index);
        if(functionPointers->tucube_IBase_tlDestroy(childModule) == -1)
            warnx("%s: %u: tucube_Module_tlDestroy() failed", __FILE__, __LINE__);
    }
    pthread_mutex_lock(coreModule->exitMutex);
    coreModule->exit = true;
    pthread_cond_signal(coreModule->exitCond);
    pthread_mutex_unlock(coreModule->exitMutex);
}

static void* tucube_Core_startWorker(void* args) {
    struct tucube_Core* core = ((void**)args)[0];
    struct tucube_Config* config = ((void**)args)[1];
    pthread_cleanup_push(tucube_Core_pthreadCleanupHandler, core);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    GENC_ARRAY_LIST_FOR_EACH(&coreModule->childModules, index) {
        struct tucube_Core_FunctionPointers* functionPointers = &GENC_ARRAY_LIST_GET(&coreModule->functionPointersList, index);
        struct tucube_Module* childModule = GENC_ARRAY_LIST_GET(&coreModule->childModules, index);
        if(functionPointers->tucube_IBase_tlInit(childModule, config, (void*[]){NULL}) == -1)
            errx(EXIT_FAILURE, "%s: %u: tucube_Module_tlInit() failed", __FILE__, __LINE__);
    }

    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGINT);
    if(pthread_sigmask(SIG_BLOCK, &signalSet, NULL) != 0)
        errx(EXIT_FAILURE, "%s: %u: pthread_sigmask() failed", __FILE__, __LINE__);

    GENC_ARRAY_LIST_FOR_EACH(&coreModule->childModules, index) {
        struct tucube_Core_FunctionPointers* functionPointers = &GENC_ARRAY_LIST_GET(&coreModule->functionPointersList, index);
        struct tucube_Module* childModule = GENC_ARRAY_LIST_GET(&coreModule->childModules, index);
        if(functionPointers->tucube_ITlService_call(childModule, (void*[]){&coreModule->serverSocket, NULL}) == -1)
            errx(EXIT_FAILURE, "%s: %u: tucube_ITlService_call() failed", __FILE__, __LINE__);
    }

    pthread_cleanup_pop(1);

    return NULL;
}

struct tucube_Core_ChildModuleNames {
    GENC_ARRAY_LIST(const char*);
};

static int tucube_ITlService_call(struct tucube_Module* module, void* args[]) {
    return 0;
}

static int tucube_IBase_init(struct tucube_Module* module, struct tucube_Config* config) {
    struct tucube_Core* coreModule = module->generic.pointer;
    if((coreModule->tucube_ITlService_call = 
    return 0;
}

static int tucube_IBase_destroy(struct tucube_Module* module) {
    return 0;
}

static int tucube_Core_initRootModule(struct tucube_Module* module, struct tucube_Config* config) {
    struct tucube_Core* coreModule = module->generic.pointer;
    TUCUBE_CONFIG_GET(config, module->name, "tucube.protocol", string, &coreModule->protocol, "TCP");
    TUCUBE_CONFIG_GET(config, module->name, "tucube.address", string, &coreModule->address, "0.0.0.0");
    TUCUBE_CONFIG_GET(config, module->name, "tucube.port", integer, &coreModule->port, 8080);
    TUCUBE_CONFIG_GET(config, module->name, "tucube.reusePort", integer, &coreModule->reusePort, 0);
    TUCUBE_CONFIG_GET(config, module->name, "tucube.backlog", integer, &coreModule->backlog, 1024);
    TUCUBE_CONFIG_GET(config, module->name, "tucube.workerCount", integer, &coreModule->workerCount, 4);
    TUCUBE_CONFIG_GET(config, module->name, "tucube.setUid", integer, &coreModule->setUid, geteuid());
    TUCUBE_CONFIG_GET(config, module->name, "tucube.setGid", integer, &coreModule->setGid, getegid());
    coreModule->exit = false;
    coreModule->exitMutex = malloc(1 * sizeof(pthread_mutex_t));
    pthread_mutex_init(coreModule->exitMutex, NULL);
    coreModule->exitCond = malloc(1 * sizeof(pthread_cond_t));
    pthread_cond_init(coreModule->exitCond, NULL);

    struct sockaddr_in serverAddressSockAddrIn;
    memset(serverAddressSockAddrIn.sin_zero, 0, 1 * sizeof(serverAddressSockAddrIn.sin_zero));
    serverAddressSockAddrIn.sin_family = AF_INET; 

    inet_aton(coreModule->address, &serverAddressSockAddrIn.sin_addr);
    serverAddressSockAddrIn.sin_port = htons(coreModule->port);

    if(strcmp(coreModule->protocol, "TCP") == 0) {
        if((coreModule->serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        if(setsockopt(coreModule->serverSocket, SOL_SOCKET, SO_REUSEADDR, &(const int){1}, sizeof(int)) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        if(setsockopt(coreModule->serverSocket, SOL_SOCKET, SO_REUSEPORT, &coreModule->reusePort, sizeof(int)) == -1)
            warn("%s: %u", __FILE__, __LINE__);
        if(bind(coreModule->serverSocket, (struct sockaddr*)&serverAddressSockAddrIn, sizeof(struct sockaddr)) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        if(listen(coreModule->serverSocket, coreModule->backlog) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    } else if(strcmp(coreModule->protocol, "UDP") == 0) {
        if((coreModule->serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        if(bind(coreModule->serverSocket, (struct sockaddr*)&serverAddressSockAddrIn, sizeof(struct sockaddr)) == -1)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    } else
        errx(EXIT_FAILURE, "%s: %u: Unknown protocol %s", __FILE__, __LINE__, coreModule->protocol);

    return 0;
}

static int tucube_Core_initChildModules(struct tucube_Module* module, struct tucube_Config* config) {
    struct tucube_Core_ChildModuleNames childModuleNames;
    GENC_ARRAY_LIST_INIT(childModuleNames);
    TUCUBE_CONFIG_GET_CHILD_MODULE_NAMES(config, module->name, &childModuleNames);
    size_t childModuleCount = GENC_ARRAY_LIST_SIZE(childModuleNames);
    GENC_TREE_NODE_INIT_CHILDREN(module, childModuleCount);
    GENC_ARRAY_LIST_FOR_EACH(childModuleNames, index) {
        struct tucube_Module* childModule = GENC_TREE_NODE_GET_CHILD(module, index);
        GENC_TREE_NODE_INIT(childModule);
        childModule->name = GENC_ARRAY_LIST_GET(childModuleNames, index);
        const char* childModulePath = NULL;
        TUCUBE_CONFIG_GET_MODULE_PATH(config, childModule->name, &childModulePath);
        if((childModule->dlHandle = dlopen(childModulePath, RTLD_LAZY | RTLD_GLOBAL)) == NULL)
            errx(EXIT_FAILURE, "%s: %u: dlopen() failed, possible causes are:\n1. Unable to find next module\n2. The next module didn't linked required shared libraries properly", __FILE__, __LINE__);
        if((childModule->tucube_IBase_init = dlsym(module->dlHandle, "tucube_IBase_init")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_IBase_init()", __FILE__, __LINE__);
        if((childModule->tucube_IBase_tlInit = dlsym(module->dlHandle, "tucube_IBase_tlInit")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_IBase_tlInit()", __FILE__, __LINE__);
        if((childModule->tucube_IBase_tlDestroy = dlsym(module->dlHandle, "tucube_IBase_tlDestroy")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_IBase_tlDestroy()", __FILE__, __LINE__);
        if((childModule->tucube_IBase_destroy = dlsym(module->dlHandle, "tucube_IBase_destroy")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_IBase_destroy()", __FILE__, __LINE__);
        childModule->tucube_IBase_init(childModule, config);
        tucube_Core_initChildModules(childModule, config);
    }
    GENC_ARRAY_LIST_FREE(childModuleNames);
    return 0;
}

static int tucube_Core_destroyChildModules(struct tucube_Module* module) {
    GENC_TREE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = GENC_TREE_NODE_GET_CHILD(module, index);
        if(childModule->tucube_IBase_destroy(childModule) == -1)
           warn("%s: %u", __FILE__, __LINE__);
        tucube_Core_destroyChildModules(childModule);
    }
    return 0;
}

static int tucube_Core_init(struct tucube_Module* module, struct tucube_Config* config) {
    tucube_Core_initRootModule(module, config);
    tucube_Core_initChildModules(module, config);
    if(setgid(coreModule->setGid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    if(setuid(coreModule->setUid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    return 0;
}

int tucube_Core_start(struct tucube_Module* module, struct tucube_Config* config) {
    tucube_Core_init(module, config);
    tucube_Core_registerSignalHandlers();
    pthread_t* workerThreads;
    pthread_attr_t coreThreadAttr;
    jmp_buf* jumpBuffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jumpBuffer) == 0) {
        pthread_key_create(&tucube_Core_tlKey, NULL);
        pthread_setspecific(tucube_Core_tlKey, jumpBuffer);

        pthread_attr_init(&coreThreadAttr);
        pthread_attr_setdetachstate(&coreThreadAttr, PTHREAD_CREATE_JOINABLE);

        workerThreads = malloc(coreModule->workerCount * sizeof(pthread_t));

        atexit(tucube_Core_exitHandler);

        void* workerArgs[2] = {module, config};
        for(size_t index = 0; index != coreModule->workerCount; ++index) {
           if(pthread_create(workerThreads + index, &coreThreadAttr, tucube_Core_startWorker, workerArgs) != 0)
                err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        }

        pthread_mutex_lock(coreModule->exitMutex);
        while(coreModule->exit != true) {
            pthread_cond_wait(coreModule->exitCond,
                 coreModule->exitMutex);
        }
        pthread_mutex_unlock(coreModule->exitMutex);

        for(size_t index = 0; index != coreModule->workerCount; ++index) {
            pthread_cancel(workerThreads[index]);
            pthread_join(workerThreads[index], NULL);
        }
        coreModule->exit = true;
    }
    free(jumpBuffer);
    pthread_key_delete(tucube_Core_tlKey);
    pthread_mutex_unlock(coreModule->exitMutex);

    if(coreModule->exit == false) {
        for(size_t index = 0; index != coreModule->workerCount; ++index) {
            pthread_cancel(workerThreads[index]);
            pthread_mutex_lock(coreModule->exitMutex);
            while(coreModule->exit != true)
                pthread_cond_wait(coreModule->exitCond, coreModule->exitMutex);
            pthread_mutex_unlock(coreModule->exitMutex);
            pthread_join(workerThreads[index], NULL);
            coreModule->exit = false;
        }
    }

    pthread_cond_destroy(coreModule->exitCond);
    free(coreModule->exitCond);
    pthread_mutex_destroy(coreModule->exitMutex);
    free(coreModule->exitMutex);
    close(coreModule->serverSocket);
    pthread_attr_destroy(&coreThreadAttr);
    free(workerThreads);
//    dlclose(coreModule->dlHandle);
    return 0;
}
