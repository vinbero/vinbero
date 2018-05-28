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
#include <vinbero_common/vinbero_common_Call.h>
#include <vinbero_common/vinbero_common_Config.h>
#include <vinbero_common/vinbero_common_Module.h>
#include "vinbero_core.h"
#include "vinbero_IMODULE.h"
#include "vinbero_IBASIC.h"

static pthread_key_t vinbero_core_tlKey;

struct vinbero_core {
    uid_t setUid;
    gid_t setGid;
};

static void vinbero_core_sigIntHandler(int signal_number) {
    VINBERO_COMMON_LOG_TRACE2();
    exit(EXIT_FAILURE);
}

static void vinbero_core_exitHandler() {
    VINBERO_COMMON_LOG_TRACE2();
    if(syscall(SYS_gettid) == getpid()) {
        jmp_buf* jumpBuffer = pthread_getspecific(vinbero_core_tlKey);
        if(jumpBuffer != NULL)
            longjmp(*jumpBuffer, 1);
    }
}

int vinbero_core_registerSignalHandlers() {
    VINBERO_COMMON_LOG_TRACE2();
    struct sigaction signalAction;
    signalAction.sa_handler = vinbero_core_sigIntHandler;
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

void vinbero_core_registerExitHandler() {
    atexit(vinbero_core_exitHandler);
}

int vinbero_core_checkConfig(struct vinbero_common_Config* config, const char* moduleId) {
    VINBERO_COMMON_LOG_TRACE2();
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
        if((ret = vinbero_core_checkConfig(config, GENC_ARRAY_LIST_GET(&childModuleIds, index))) < 0) {
            GENC_ARRAY_LIST_FREE(&childModuleIds);
            return ret;
        }
    }

    GENC_ARRAY_LIST_FREE(&childModuleIds);

    return 0;
}

int vinbero_core_initLocalModule(struct vinbero_common_Module* module, struct vinbero_common_Config* config) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    module->childrenRequired = true;
    module->localModule.pointer = malloc(1 * sizeof(struct vinbero_core));
    struct vinbero_core* localModule = module->localModule.pointer;
    if((ret = vinbero_common_Config_getInt(config, module, "vinbero.setUid", &localModule->setUid, geteuid())) < 0)
        return ret;
    if((ret = vinbero_common_Config_getInt(config, module, "vinbero.setGid", &localModule->setGid, getegid())) < 0)
        return ret;
    return 0;
}

int vinbero_core_loadChildModules(struct vinbero_common_Module* module, struct vinbero_common_Module* parentModule, const char* moduleId, struct vinbero_common_Config* config) {
    VINBERO_COMMON_LOG_TRACE2();
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
        if((ret = vinbero_core_loadChildModules(childModule, module, GENC_ARRAY_LIST_GET(&childModuleIds, index), config)) < 0) {
            GENC_ARRAY_LIST_FREE(&childModuleIds);
            return ret;
        }
    }
    GENC_ARRAY_LIST_FREE(&childModuleIds);

    return 0;
}

int vinbero_core_initChildModules(struct vinbero_common_Module* module, struct vinbero_common_Config* config) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct vinbero_IMODULE_Interface childInterface;
        childModule->childrenRequired = true;
        VINBERO_COMMON_CALL(IMODULE, init, childModule, &ret, childModule, config, (void*[]){NULL});
        if(ret < 0)
            return ret;
        if(childModule->childrenRequired == true && GENC_TREE_NODE_GET_CHILD_COUNT(childModule) == 0) {
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
        if((ret = vinbero_core_initChildModules(childModule, config)) < 0)
            return ret;
    }
    return 0;
}

int vinbero_core_rInitChildModules(struct vinbero_common_Module* module, struct vinbero_common_Config* config) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if((ret = vinbero_core_initChildModules(childModule, config)) < 0)
            return ret;
        VINBERO_COMMON_CALL(IMODULE, rInit, childModule, &ret, childModule, config, (void*[]){NULL});
        if(ret < 0)
            return ret;
    }
    return 0;
}

int vinbero_core_destroyChildModules(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        VINBERO_COMMON_CALL(IMODULE, destroy, childModule, &ret, childModule);
        if(ret < 0)
            return ret;
        if((ret = vinbero_core_destroyChildModules(childModule)) < 0)
            return ret;
    }
    return 0;
}

int vinbero_core_rDestroyChildModules(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        if((ret = vinbero_core_rDestroyChildModules(childModule)) < 0)
            return ret;
        VINBERO_COMMON_CALL(IMODULE, rDestroy, childModule, &ret, childModule);
        if(ret < 0)
            return ret;
    }
    GENC_TREE_NODE_FREE(module);
    return 0;
}

int vinbero_core_setGid(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    struct vinbero_core* localModule = module->localModule.pointer;
    if(setgid(localModule->setGid) < 0) {
        VINBERO_COMMON_LOG_ERROR("setgid(...) failed");
        return -errno;
    }
    return 0;
}

int vinbero_core_setUid(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    struct vinbero_core* localModule = module->localModule.pointer;
    if(setgid(localModule->setUid) < 0) {
        VINBERO_COMMON_LOG_ERROR("setuid(...) failed");
        return -errno;
    }
    return 0;
}

int vinbero_core_start(struct vinbero_common_Module* module, struct vinbero_common_Config* config) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    jmp_buf* jumpBuffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jumpBuffer) == 0) {
        pthread_key_create(&vinbero_core_tlKey, NULL);
        pthread_setspecific(vinbero_core_tlKey, jumpBuffer);
        GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
            struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
            VINBERO_COMMON_CALL(IBASIC, service, childModule, &ret, childModule, (void*[]){NULL});
            if(ret < 0)
                return ret;
        }
    }

    free(jumpBuffer);
    pthread_key_delete(vinbero_core_tlKey);
    vinbero_core_destroyChildModules(module);
    vinbero_core_rDestroyChildModules(module);

//    dlclose(localModule->dlHandle);
    return 0;
}
