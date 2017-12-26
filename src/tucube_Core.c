#include <dlfcn.h>
#include <err.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
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


static int tucube_Core_initLocalModule(struct tucube_Module* module, struct tucube_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_Core* localModule = module->localModule.pointer;
    TUCUBE_CONFIG_GET(config, module->id, "tucube.setUid", integer, &localModule->setUid, geteuid());
    TUCUBE_CONFIG_GET(config, module->id, "tucube.setGid", integer, &localModule->setGid, getegid());
    return 0;
}

static int tucube_Core_preInitChildModules(struct tucube_Module* module, struct tucube_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_Module_Ids childModuleIds;
    GENC_ARRAY_LIST_INIT(&childModuleIds);
    TUCUBE_CONFIG_GET_CHILD_MODULE_IDS(config, module->id, &childModuleIds);
    // check if 
    size_t childModuleCount = GENC_ARRAY_LIST_SIZE(&childModuleIds);
    GENC_TREE_NODE_INIT_CHILDREN(module, childModuleCount);
    GENC_TREE_NODE_ZERO_CHILDREN(module);
    GENC_ARRAY_LIST_FOR_EACH(&childModuleIds, index) {
        GENC_TREE_NODE_ADD_EMPTY_CHILD(module);
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        GENC_TREE_NODE_INIT(childModule);
        GENC_TREE_NODE_SET_PARENT(childModule, module);
        childModule->id = GENC_ARRAY_LIST_GET(&childModuleIds, index);
        const char* childModulePath = NULL;
        TUCUBE_CONFIG_GET_MODULE_PATH(config, childModule->id, &childModulePath);
        if((childModule->dlHandle = dlopen(childModulePath, RTLD_LAZY | RTLD_GLOBAL)) == NULL)
            errx(EXIT_FAILURE, "%s: %u: dlopen() failed, possible causes are:\n1. Unable to find next module\n2. The next module didn't linked required shared libraries properly", __FILE__, __LINE__);
        childModule->interface = malloc(1 * sizeof(struct tucube_Core_Interface)); // An interface should be a struct consisting of function pointers only.
         if((childModule->tucube_IModule_init = dlsym(childModule->dlHandle, "tucube_IModule_init")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_IModule_init()", __FILE__, __LINE__);
        if((childModule->tucube_IModule_destroy = dlsym(childModule->dlHandle, "tucube_IModule_destroy")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_IModule_destroy()", __FILE__, __LINE__);
        tucube_Core_preInitChildModules(childModule, config);
    }
    GENC_ARRAY_LIST_FREE(&childModuleIds);
    return 0;
}

static int tucube_Core_initChildModules(struct tucube_Module* module, struct tucube_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if(childModule->tucube_IModule_init(childModule, config, (void*[]){NULL}) == -1)
            return -1;
        if(tucube_Core_initChildModules(childModule, config) == -1)
            return -1;
    }
    return 0;
}

static int tucube_Core_destroyChildModules(struct tucube_Module* module) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        tucube_Core_destroyChildModules(childModule);
    }
    if(module->tucube_IModule_destroy != NULL && module->tucube_IModule_destroy(module) == -1) // should fix this later
       warn("%s: %u", __FILE__, __LINE__);
    GENC_TREE_NODE_FREE_CHILDREN(module);
    return 0;
}

static int tucube_Core_init(struct tucube_Module* module, struct tucube_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_Core* localModule = module->localModule.pointer;
    tucube_Core_initLocalModule(module, config);
    tucube_Core_preInitChildModules(module, config);
    if(tucube_Core_initChildModules(module, config) == -1);
        errx(EXIT_FAILURE, "%s: %u: tucube_Core_initChildModules() failed", __FILE__, __LINE__);
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
    tucube_Core_registerSignalHandlers();
    atexit(tucube_Core_exitHandler);
    jmp_buf* jumpBuffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jumpBuffer) == 0) {
        pthread_key_create(&tucube_Core_tlKey, NULL);
        pthread_setspecific(tucube_Core_tlKey, jumpBuffer);
        GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
            struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
            struct tucube_Core_Interface* moduleInterface = childModule->interface;
            if((moduleInterface->tucube_ICore_service = dlsym(childModule->dlHandle, "tucube_ICore_service")) == NULL)
                errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_ICore_service()", __FILE__, __LINE__);
            if(moduleInterface->tucube_ICore_service(childModule, (void*[]){NULL}) == -1)
                errx(EXIT_FAILURE, "%s: %u: tucube_ICore_service() failed", __FILE__, __LINE__);
        }
    }
    free(jumpBuffer);
    pthread_key_delete(tucube_Core_tlKey);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        free(childModule->interface);
    }
    tucube_Core_destroyChildModules(module);

//    dlclose(localModule->dlHandle);
    return 0;
}
