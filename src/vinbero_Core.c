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
#include <vinbero_common/vinbero_common_Log.h>
#include <vinbero_common/vinbero_common_Config.h>
#include <vinbero_common/vinbero_common_Module.h>
#include "vinbero_Core.h"
#include "vinbero_IModule.h"
#include "vinbero_IBasic.h"

static pthread_key_t vinbero_Core_tlKey;

struct vinbero_Core {
    uid_t setUid;
    gid_t setGid;
};

static void vinbero_Core_sigIntHandler(int signal_number) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    exit(EXIT_FAILURE);
}

static void vinbero_Core_exitHandler() {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    if(syscall(SYS_gettid) == getpid()) {
        jmp_buf* jumpBuffer = pthread_getspecific(vinbero_Core_tlKey);
        if(jumpBuffer != NULL)
            longjmp(*jumpBuffer, 1);
    }
}

int vinbero_Core_registerSignalHandlers() {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
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

void vinbero_Core_registerExitHandler() {
    atexit(vinbero_Core_exitHandler);
}

int vinbero_Core_checkConfig(struct vinbero_common_Config* config, const char* moduleId) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    if((ret = vinbero_common_Config_check(config, moduleId)) < 0) {
        VINBERO_COMMON_LOG_ERROR("Module %s has wrong config or doesn't exist", moduleId);
        return ret;
    }
    struct vinbero_common_Module_Ids childModuleIds;

    GENC_ARRAY_LIST_INIT(&childModuleIds);

    if((ret = vinbero_common_Config_getChildModuleIds(config, moduleId, &childModuleIds)) < 0) {
        GENC_ARRAY_LIST_FREE(&childModuleIds);
        return ret;
    }


    GENC_ARRAY_LIST_FOR_EACH(&childModuleIds, index) {
        if((ret = vinbero_Core_checkConfig(config, GENC_ARRAY_LIST_GET(&childModuleIds, index))) < 0) {
            GENC_ARRAY_LIST_FREE(&childModuleIds);
            return ret;
        }
    }

    GENC_ARRAY_LIST_FREE(&childModuleIds);

    return 0;
}

int vinbero_Core_initLocalModule(struct vinbero_common_Module* module, struct vinbero_common_Config* config) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    module->needsChildren = true;
    module->localModule.pointer = malloc(1 * sizeof(struct vinbero_Core));
    struct vinbero_Core* localModule = module->localModule.pointer;
    if((ret = vinbero_common_Config_getInt(config, module, "vinbero.setUid", &localModule->setUid, geteuid())) < 0)
        return ret;
    if((ret = vinbero_common_Config_getInt(config, module, "vinbero.setGid", &localModule->setGid, getegid())) < 0)
        return ret;
    return 0;
}

int vinbero_Core_loadChildModules(struct vinbero_common_Module* module, struct vinbero_common_Module* parentModule, const char* moduleId, struct vinbero_common_Config* config) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    struct vinbero_common_Module_Ids childModuleIds;
    GENC_ARRAY_LIST_INIT(&childModuleIds);
    if((ret = vinbero_common_Config_getChildModuleIds(config, moduleId, &childModuleIds)) < 0)
        return ret;
    size_t childModuleCount = GENC_ARRAY_LIST_SIZE(&childModuleIds);
    GENC_TREE_NODE_INIT3(module, childModuleCount);
    GENC_TREE_NODE_SET_PARENT(module, parentModule);
    module->id = moduleId;
    if(parentModule != NULL) {
        if((ret = vinbero_common_Module_dlopen(module, config)) < 0) {
            VINBERO_COMMON_LOG_ERROR("%s", fastdl_error());
            return ret;
        }
    }
    GENC_ARRAY_LIST_FOR_EACH(&childModuleIds, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if((ret = vinbero_Core_loadChildModules(childModule, module, GENC_ARRAY_LIST_GET(&childModuleIds, index), config)) < 0) {
            GENC_ARRAY_LIST_FREE(&childModuleIds);
            return ret;
        }
    }
    GENC_ARRAY_LIST_FREE(&childModuleIds);

    return 0;
}

int vinbero_Core_initChildModules(struct vinbero_common_Module* module, struct vinbero_common_Config* config) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct vinbero_IModule_Interface childInterface;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &ret);
        if(ret < 0) {
            VINBERO_COMMON_LOG_ERROR("%s", fastdl_error());
            return ret;
        }
        childModule->needsChildren = true;
        if((ret = childInterface.vinbero_IModule_init(childModule, config, (void*[]){NULL})) < 0)
            return ret;
        if(childModule->needsChildren == true && GENC_TREE_NODE_GET_CHILD_COUNT(childModule) == 0) {
            VINBERO_COMMON_LOG_ERROR("module %s must have at least one child", childModule->name);
            return VINBERO_COMMON_EINVAL;
        }
        if(childModule->name == NULL) {
            VINBERO_COMMON_LOG_ERROR("Module %s has no name", childModule->id);
            return VINBERO_COMMON_EINVAL;
        }
        if(childModule->version == NULL) {
            VINBERO_COMMON_LOG_ERROR("Module %s has no version", childModule->id);
            return VINBERO_COMMON_EINVAL;
        }
        if((ret = vinbero_Core_initChildModules(childModule, config)) < 0)
            return ret;
    }
    return 0;
}

int vinbero_Core_rInitChildModules(struct vinbero_common_Module* module, struct vinbero_common_Config* config) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if((ret = vinbero_Core_initChildModules(childModule, config)) < 0)
            return ret;
        struct vinbero_IModule_Interface childInterface;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &ret);
        if(ret < 0) {
            VINBERO_COMMON_LOG_ERROR("%s", fastdl_error());
            return ret;
        }
        if((ret = childInterface.vinbero_IModule_rInit(childModule, config, (void*[]){NULL})) < 0)
            return ret;
    }
    return 0;
}

int vinbero_Core_destroyChildModules(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct vinbero_IModule_Interface childInterface;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &ret);
        if(ret < 0) {
            VINBERO_COMMON_LOG_ERROR("%s", fastdl_error());
            return ret;
        }
        if((ret = childInterface.vinbero_IModule_destroy(module)) < 0)
            return ret;
        if((ret = vinbero_Core_destroyChildModules(childModule)) < 0)
            return ret;
    }
    return 0;
}

int vinbero_Core_rDestroyChildModules(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if((ret = vinbero_Core_rDestroyChildModules(childModule)) < 0)
            return ret;
        struct vinbero_IModule_Interface childInterface;
        VINBERO_IMODULE_DLSYM(&childInterface, &childModule->dlHandle, &ret);
        if(ret < 0) {
            VINBERO_COMMON_LOG_ERROR("%s", fastdl_error());
            return ret;
        }
        if((ret = childInterface.vinbero_IModule_rDestroy(module)) < 0)
            return ret;
    }
    GENC_TREE_NODE_FREE(module);
    return 0;
}

int vinbero_Core_setGid(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    struct vinbero_Core* localModule = module->localModule.pointer;
    if(setgid(localModule->setGid) < 0) {
        VINBERO_COMMON_LOG_ERROR("setgid(...) failed");
        return -errno;
    }
    return 0;
}

int vinbero_Core_setUid(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    struct vinbero_Core* localModule = module->localModule.pointer;
    if(setgid(localModule->setUid) < 0) {
        VINBERO_COMMON_LOG_ERROR("setuid(...) failed");
        return -errno;
    }
    return 0;
}

int vinbero_Core_start(struct vinbero_common_Module* module, struct vinbero_common_Config* config) {
    VINBERO_COMMON_LOG_TRACE("in %s(...)", __FUNCTION__);
    int ret;
    jmp_buf* jumpBuffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jumpBuffer) == 0) {
        pthread_key_create(&vinbero_Core_tlKey, NULL);
        pthread_setspecific(vinbero_Core_tlKey, jumpBuffer);
        GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
            struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
            struct vinbero_IBasic_Interface childInterface;
            VINBERO_IBASIC_DLSYM(&childInterface, &childModule->dlHandle, &ret);
            if(ret < 0) {
                VINBERO_COMMON_LOG_ERROR("%s", fastdl_error());              
                return ret;
            }
            if((ret = childInterface.vinbero_IBasic_service(childModule, (void*[]){NULL})) < 0) {
                VINBERO_COMMON_LOG_ERROR("vinbero_IBasic_service(...) failed");
                return ret;
            }
        }
    }

    free(jumpBuffer);
    pthread_key_delete(vinbero_Core_tlKey);
/*
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
    }
*/
    vinbero_Core_destroyChildModules(module);
    vinbero_Core_rDestroyChildModules(module);

//    dlclose(localModule->dlHandle);
    return 0;
}
