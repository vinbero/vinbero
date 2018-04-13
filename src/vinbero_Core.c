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
#include <libgenc/genc_Tree.h>
#include <libgenc/genc_ArrayList.h>
#include "vinbero_Core.h"
#include "vinbero_IModule.h"
#include "vinbero_IBasic.h"
#include "vinbero_Module.h"
#include "vinbero_Config.h"

static pthread_key_t vinbero_Core_tlKey;

struct vinbero_Core {
    uid_t setUid;
    gid_t setGid;
};

static void vinbero_Core_sigIntHandler(int signal_number) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    exit(EXIT_FAILURE);
}

static void vinbero_Core_exitHandler() {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    if(syscall(SYS_gettid) == getpid()) {
        jmp_buf* jumpBuffer = pthread_getspecific(vinbero_Core_tlKey);
        if(jumpBuffer != NULL)
            longjmp(*jumpBuffer, 1);
    }
}

static int vinbero_Core_registerSignalHandlers() {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct sigaction signalAction;
    signalAction.sa_handler = vinbero_Core_sigIntHandler;
    signalAction.sa_flags = SA_RESTART;
    if(sigfillset(&signalAction.sa_mask) == -1)
        return -1;
    if(sigaction(SIGINT, &signalAction, NULL) == -1)
        return -1;
    signalAction.sa_handler = SIG_IGN;
    signalAction.sa_flags = SA_RESTART;
    if(sigfillset(&signalAction.sa_mask) == -1)
        return -1;
    if(sigaction(SIGPIPE, &signalAction, NULL) == -1)
        return -1;
}

static int vinbero_Core_checkConfig(struct vinbero_Config* config, const char* moduleId) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    int errorVariable;
    VINBERO_CONFIG_CHECK(config, moduleId, &errorVariable);
    if(errorVariable == 1) {
        warnx("%s: %u: %s module %s has wrong config or doesn't exist", __FILE__, __LINE__, __FUNCTION__, moduleId);
        return -1;
    }
    struct vinbero_Module_Ids childModuleIds;
    GENC_ARRAY_LIST_INIT(&childModuleIds);
    VINBERO_CONFIG_GET_CHILD_MODULE_IDS(config, moduleId, &childModuleIds);
    GENC_ARRAY_LIST_FOR_EACH(&childModuleIds, index) {
        if(vinbero_Core_checkConfig(config, GENC_ARRAY_LIST_GET(&childModuleIds, index)) == -1) {
            GENC_ARRAY_LIST_FREE(&childModuleIds);
            return -1;
        }
    }
    GENC_ARRAY_LIST_FREE(&childModuleIds);
    return 0;
}

static int vinbero_Core_initLocalModule(struct vinbero_Module* module, struct vinbero_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    module->localModule.pointer = malloc(1 * sizeof(struct vinbero_Core));
    struct vinbero_Core* localModule = module->localModule.pointer;
    VINBERO_CONFIG_GET(config, module, "vinbero.setUid", integer, &localModule->setUid, geteuid());
    VINBERO_CONFIG_GET(config, module, "vinbero.setGid", integer, &localModule->setGid, getegid());
    return 0;
}

static int vinbero_Core_loadChildModules(struct vinbero_Module* module, struct vinbero_Module* parentModule, const char* moduleId, struct vinbero_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct vinbero_Module_Ids childModuleIds;
    GENC_ARRAY_LIST_INIT(&childModuleIds);
    VINBERO_CONFIG_GET_CHILD_MODULE_IDS(config, moduleId, &childModuleIds);
    size_t childModuleCount = GENC_ARRAY_LIST_SIZE(&childModuleIds);

    GENC_TREE_NODE_INIT3(module, childModuleCount);
    GENC_TREE_NODE_SET_PARENT(module, parentModule);
    module->id = moduleId;
    if(parentModule != NULL) {
        int errorVariable;
        VINBERO_MODULE_DLOPEN(config, module, &errorVariable);
        if(errorVariable == 1)
            return -1;
    }
    GENC_ARRAY_LIST_FOR_EACH(&childModuleIds, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if(vinbero_Core_loadChildModules(childModule, module, GENC_ARRAY_LIST_GET(&childModuleIds, index), config) == -1) {
            GENC_ARRAY_LIST_FREE(&childModuleIds);
            return -1;
        }
    }
    GENC_ARRAY_LIST_FREE(&childModuleIds);
    return 0;
}

static int vinbero_Core_initChildModules(struct vinbero_Module* module, struct vinbero_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct vinbero_IModule_Interface childInterface;
        int errorVariable;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &errorVariable);
        if(errorVariable == 1)
            return -1;
        if(childInterface.vinbero_IModule_init(childModule, config, (void*[]){NULL}) == -1)
            return -1;
        if(childModule->name == NULL) {
            warnx("%s: %u: %s: module %s has no name", __FILE__, __LINE__, __FUNCTION__, childModule->id);
            return -1;
        }
        if(childModule->version == NULL) {
            warnx("%s: %u: %s: module %s has no version", __FILE__, __LINE__, __FUNCTION__, childModule->id);
            return -1;
        }
        if(vinbero_Core_initChildModules(childModule, config) == -1)
            return -1;
    }
    return 0;
}

static int vinbero_Core_rInitChildModules(struct vinbero_Module* module, struct vinbero_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if(vinbero_Core_initChildModules(childModule, config) == -1)
            return -1;
        struct vinbero_IModule_Interface childInterface;
        int errorVariable;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &errorVariable);
        if(errorVariable == 1)
            return -1;
        if(childInterface.vinbero_IModule_rInit(childModule, config, (void*[]){NULL}) == -1)
            return -1;
    }
    return 0;
}

static int vinbero_Core_destroyChildModules(struct vinbero_Module* module) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
/*    if(module->vinbero_IModule_destroy != NULL && module->vinbero_IModule_destroy(module) == -1) { // should fix this later
        warn("%s: %u", __FILE__, __LINE__);
        return -1;
    }
*/
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct vinbero_IModule_Interface childInterface;
        int errorVariable;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &errorVariable);
        childInterface.vinbero_IModule_destroy(module);
        if(vinbero_Core_destroyChildModules(childModule) == -1)
            return -1;
    }
    return 0;
}

static int vinbero_Core_rDestroyChildModules(struct vinbero_Module* module) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        vinbero_Core_rDestroyChildModules(childModule);
        struct vinbero_IModule_Interface childInterface;
        int errorVariable;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &errorVariable);
        childInterface.vinbero_IModule_rDestroy(module);
    }
/*    if(module->vinbero_IModule_rDestroy != NULL && module->vinbero_IModule_rDestroy(module) == -1) // should fix this later
       warn("%s: %u", __FILE__, __LINE__);
*/
    GENC_TREE_NODE_FREE(module);
    return 0;
}

static int vinbero_Core_initCoreModule(struct vinbero_Module** module, struct vinbero_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    if(vinbero_Core_checkConfig(config, "core") == -1)
        errx(EXIT_FAILURE, "%s: %u: vinbero_Core_checkConfig() failed", __FILE__, __LINE__);
    *module = calloc(1, sizeof(struct vinbero_Module));
    if(vinbero_Core_loadChildModules(*module, NULL, "core", config) == -1) {
        errx(EXIT_FAILURE, "%s: %u: vinbero_Core_loadChildModules() failed", __FILE__, __LINE__);
        // destroy child modules
    }
    if(vinbero_Core_initLocalModule(*module, config) == -1)
        errx(EXIT_FAILURE, "%s: %u: vinbero_Core_initLocalModule() failed", __FILE__, __LINE__);

    if(vinbero_Core_initChildModules(*module, config) == -1) {
        errx(EXIT_FAILURE, "%s: %u: vinbero_Core_initChildModules() failed", __FILE__, __LINE__);
    }
    if(vinbero_Core_rInitChildModules(*module, config) == -1) {
        errx(EXIT_FAILURE, "%s: %u: vinbero_Core_initChildModules() failed", __FILE__, __LINE__);
    }
    struct vinbero_Core* localModule = (*module)->localModule.pointer;
    if(setgid(localModule->setGid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    if(setuid(localModule->setUid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    return 0;
}

int vinbero_Core_start(struct vinbero_Config* config) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct vinbero_Module* module;
    vinbero_Core_initCoreModule(&module, config);
    struct vinbero_Core* localModule = module->localModule.pointer;
    if(vinbero_Core_registerSignalHandlers() == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    atexit(vinbero_Core_exitHandler);
    jmp_buf* jumpBuffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jumpBuffer) == 0) {
        pthread_key_create(&vinbero_Core_tlKey, NULL);
        pthread_setspecific(vinbero_Core_tlKey, jumpBuffer);

        GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
            struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
            struct vinbero_IBasic_Interface childInterface;
            int errorVariable;
            VINBERO_IBASIC_DLSYM(&childInterface, &childModule->dlHandle, &errorVariable);
            if(errorVariable == 1)
                return -1;
            if(childInterface.vinbero_IBasic_service(childModule, (void*[]){NULL}) == -1)
                errx(EXIT_FAILURE, "%s: %u: vinbero_IBasic_service() failed", __FILE__, __LINE__);
        }

    }

    free(jumpBuffer);
    pthread_key_delete(vinbero_Core_tlKey);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
//        free(&childModule->interface);
    }
    vinbero_Core_destroyChildModules(module);
    vinbero_Core_rDestroyChildModules(module);

//    dlclose(localModule->dlHandle);
    return 0;
}
