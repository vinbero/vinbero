#include <errno.h>
#include <fastdl.h>
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
#include <vinbero_log.h>
#include "vinbero_Module.h"
#include "vinbero_Config.h"

static pthread_key_t vinbero_Core_tlKey;

struct vinbero_Core {
    uid_t setUid;
    gid_t setGid;
};

static void vinbero_Core_sigIntHandler(int signal_number) {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    exit(EXIT_FAILURE);
}

static void vinbero_Core_exitHandler() {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    if(syscall(SYS_gettid) == getpid()) {
        jmp_buf* jumpBuffer = pthread_getspecific(vinbero_Core_tlKey);
        if(jumpBuffer != NULL)
            longjmp(*jumpBuffer, 1);
    }
}

static int vinbero_Core_registerSignalHandlers() {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    struct sigaction signalAction;
    signalAction.sa_handler = vinbero_Core_sigIntHandler;
    signalAction.sa_flags = SA_RESTART;
    if(sigfillset(&signalAction.sa_mask) == -1)
        return -errno;
    if(sigaction(SIGINT, &signalAction, NULL) == -1)
        return -errno;
    signalAction.sa_handler = SIG_IGN;
    signalAction.sa_flags = SA_RESTART;
    if(sigfillset(&signalAction.sa_mask) == -1)
        return -errno;
    if(sigaction(SIGPIPE, &signalAction, NULL) == -1)
        return -errno;
}

static int vinbero_Core_checkConfig(struct vinbero_Config* config, const char* moduleId) {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    VINBERO_CONFIG_CHECK(config, moduleId, &ret);
    if(ret < 0) {
        VINBERO_LOG_ERROR("Module %s has wrong config or doesn't exist", moduleId);
        return ret;
    }
    struct vinbero_Module_Ids childModuleIds;
    GENC_ARRAY_LIST_INIT(&childModuleIds);
    VINBERO_CONFIG_GET_CHILD_MODULE_IDS(config, moduleId, &childModuleIds);
    GENC_ARRAY_LIST_FOR_EACH(&childModuleIds, index) {
        if((ret = vinbero_Core_checkConfig(config, GENC_ARRAY_LIST_GET(&childModuleIds, index))) < 0) {
            GENC_ARRAY_LIST_FREE(&childModuleIds);
            return ret;
        }
    }
    GENC_ARRAY_LIST_FREE(&childModuleIds);
    return 0;
}

static int vinbero_Core_initLocalModule(struct vinbero_Module* module, struct vinbero_Config* config) {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    module->localModule.pointer = malloc(1 * sizeof(struct vinbero_Core));
    struct vinbero_Core* localModule = module->localModule.pointer;
    VINBERO_CONFIG_GET(config, module, "vinbero.setUid", integer, &localModule->setUid, geteuid());
    VINBERO_CONFIG_GET(config, module, "vinbero.setGid", integer, &localModule->setGid, getegid());
    return 0;
}

static int vinbero_Core_loadChildModules(struct vinbero_Module* module, struct vinbero_Module* parentModule, const char* moduleId, struct vinbero_Config* config) {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    struct vinbero_Module_Ids childModuleIds;
    GENC_ARRAY_LIST_INIT(&childModuleIds);
    VINBERO_CONFIG_GET_CHILD_MODULE_IDS(config, moduleId, &childModuleIds);
    size_t childModuleCount = GENC_ARRAY_LIST_SIZE(&childModuleIds);

    GENC_TREE_NODE_INIT3(module, childModuleCount);
    GENC_TREE_NODE_SET_PARENT(module, parentModule);
    module->id = moduleId;
    if(parentModule != NULL) {
        VINBERO_MODULE_DLOPEN(config, module, &ret);
        if(ret < 0) {
            VINBERO_LOG_ERROR("%s", fastdl_error());
            return ret;
        }
    }
    GENC_ARRAY_LIST_FOR_EACH(&childModuleIds, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if((ret = vinbero_Core_loadChildModules(childModule, module, GENC_ARRAY_LIST_GET(&childModuleIds, index), config)) < 0) {
            GENC_ARRAY_LIST_FREE(&childModuleIds);
            return ret;
        }
    }
    GENC_ARRAY_LIST_FREE(&childModuleIds);
    return 0;
}

static int vinbero_Core_initChildModules(struct vinbero_Module* module, struct vinbero_Config* config) {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct vinbero_IModule_Interface childInterface;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &ret);
        if(ret < 0) {
            VINBERO_LOG_ERROR("%s", fastdl_error());
            return ret;
        }
        if((ret = childInterface.vinbero_IModule_init(childModule, config, (void*[]){NULL})) < 0)
            return ret;
        if(childModule->name == NULL) {
            VINBERO_LOG_ERROR("Module %s has no name", childModule->id);
            return VINBERO_EINVAL;
        }
        if(childModule->version == NULL) {
            VINBERO_LOG_ERROR("Module %s has no version", childModule->id);
            return VINBERO_EINVAL;
        }
        if((ret = vinbero_Core_initChildModules(childModule, config)) < 0)
            return ret;
    }
    return 0;
}

static int vinbero_Core_rInitChildModules(struct vinbero_Module* module, struct vinbero_Config* config) {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if((ret = vinbero_Core_initChildModules(childModule, config)) < 0)
            return ret;
        struct vinbero_IModule_Interface childInterface;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &ret);
        if(ret < 0) {
            VINBERO_LOG_ERROR("%s", fastdl_error());
            return ret;
        }
        if((ret = childInterface.vinbero_IModule_rInit(childModule, config, (void*[]){NULL})) < 0)
            return ret;
    }
    return 0;
}

static int vinbero_Core_destroyChildModules(struct vinbero_Module* module) {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct vinbero_IModule_Interface childInterface;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &ret);
        if(ret < 0) {
            VINBERO_LOG_ERROR("%s", fastdl_error());
            return ret;
        }
        if((ret = childInterface.vinbero_IModule_destroy(module)) < 0)
            return ret;
        if((ret = vinbero_Core_destroyChildModules(childModule)) < 0)
            return ret;
    }
    return 0;
}

static int vinbero_Core_rDestroyChildModules(struct vinbero_Module* module) {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if((ret = vinbero_Core_rDestroyChildModules(childModule)) < 0)
            return ret;
        struct vinbero_IModule_Interface childInterface;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &ret);
        if(ret < 0) {
            VINBERO_LOG_ERROR("%s", fastdl_error());
            return ret;
        }
        if((ret = childInterface.vinbero_IModule_rDestroy(module)) < 0)
            return ret;
    }
    GENC_TREE_NODE_FREE(module);
    return 0;
}

static int vinbero_Core_initCoreModule(struct vinbero_Module** module, struct vinbero_Config* config) {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    if((ret = vinbero_Core_checkConfig(config, "core")) < 0) {
        VINBERO_LOG_ERROR("vinbero_Core_checkConfig(...) failed");
        return ret; 
    }
    *module = calloc(1, sizeof(struct vinbero_Module));
    if((ret = vinbero_Core_loadChildModules(*module, NULL, "core", config)) < 0) {
        VINBERO_LOG_ERROR("vinbero_Core_loadChildModules(...) failed");
        return ret;
    }
    if((ret = vinbero_Core_initLocalModule(*module, config)) < 0) {
        VINBERO_LOG_ERROR("vinbero_Core_initLocalModule(...) failed");
        return ret;
    }
    if((ret = vinbero_Core_initLocalModule(*module, config)) < 0)
        VINBERO_LOG_ERROR("vinbero_Core_initLocalModule(...) failed");

    if((ret = vinbero_Core_initChildModules(*module, config)) < 0) {
        VINBERO_LOG_ERROR("vinbero_Core_initChildModules(...) failed");
        return ret;
    }
    if((ret = vinbero_Core_rInitChildModules(*module, config)) < 0) {
        VINBERO_LOG_ERROR("vinbero_Core_rInitChildModules(...) failed");
        return ret;
    }

    struct vinbero_Core* localModule = (*module)->localModule.pointer;
    if(setgid(localModule->setGid) < 0) {
        VINBERO_LOG_ERROR("setgid(...) failed");
        return -errno;
    }
    if(setuid(localModule->setUid) < 0) {
        VINBERO_LOG_ERROR("setuid(...) failed");
        return -errno;
    }
    return 0;
}

int vinbero_Core_start(struct vinbero_Config* config) {
    VINBERO_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    struct vinbero_Module* module;
    if((ret = vinbero_Core_initCoreModule(&module, config)) < 0) {
        VINBERO_LOG_ERROR("vinbero_Core_initCoreModule(...) failed");
        return ret;
    }
    struct vinbero_Core* localModule = module->localModule.pointer;
    if((ret = vinbero_Core_registerSignalHandlers()) < 0) {
        VINBERO_LOG_ERROR("vinbero_Core_registerSignalHandlers(...) failed");
        return ret;
    }
    atexit(vinbero_Core_exitHandler);
    jmp_buf* jumpBuffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jumpBuffer) == 0) {
        pthread_key_create(&vinbero_Core_tlKey, NULL);
        pthread_setspecific(vinbero_Core_tlKey, jumpBuffer);
        GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
            struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
            struct vinbero_IBasic_Interface childInterface;
            VINBERO_IBASIC_DLSYM(&childInterface, &childModule->dlHandle, &ret);
            if(ret < 0) {
                VINBERO_LOG_ERROR("%s", fastdl_error());              
                return ret;
            }
            if((ret = childInterface.vinbero_IBasic_service(childModule, (void*[]){NULL})) < 0) {
                VINBERO_LOG_ERROR("vinbero_IBasic_service(...) failed");
                return ret;
            }
        }
    }

    free(jumpBuffer);
    pthread_key_delete(vinbero_Core_tlKey);
/*
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
    }
*/
    vinbero_Core_destroyChildModules(module);
    vinbero_Core_rDestroyChildModules(module);

//    dlclose(localModule->dlHandle);
    return 0;
}
