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
#include <libgenc/genc_Alist.h>
#include <vinbero_com/vinbero_com_Log.h>
#include <vinbero_com/vinbero_com_Call.h>
#include <vinbero_com/vinbero_com_Config.h>
#include <vinbero_com/vinbero_com_Module.h>
#include <vinbero_com/vinbero_com_Status.h>
#include <vinbero_com/vinbero_com_Object.h>
#include <vinbero_iface_MODULE/vinbero_iface_MODULE.h>
#include <vinbero_iface_BASIC/vinbero_iface_BASIC.h>
#include "vinbero_core.h"
#include "config.h"

VINBERO_COM_MODULE_META_NAME("vinbero_core")
VINBERO_COM_MODULE_META_LICENSE("MPL-2.0")
VINBERO_COM_MODULE_META_VERSION(VINBERO_VERSION_MAJOR, VINBERO_VERSION_MINOR, VINBERO_VERSION_PATCH)
VINBERO_COM_MODULE_META_IN_IFACES("")
VINBERO_COM_MODULE_META_OUT_IFACES("BASIC")
VINBERO_COM_MODULE_META_CHILD_COUNT(-1, -1)

static pthread_key_t vinbero_core_tlKey;

struct vinbero_core {
    uid_t setUid;
    gid_t setGid;
};

static void vinbero_core_sigIntHandler(int signal_number) {
    VINBERO_COM_LOG_TRACE2();
    exit(EXIT_FAILURE);
}

static void vinbero_core_exitHandler() {
    VINBERO_COM_LOG_TRACE2();
    if(syscall(SYS_gettid) == getpid()) {
        jmp_buf* jumpBuffer = pthread_getspecific(vinbero_core_tlKey);
        if(jumpBuffer != NULL)
            longjmp(*jumpBuffer, 1);
    }
}

int vinbero_core_registerSignalHandlers() {
    VINBERO_COM_LOG_TRACE2();
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
    return VINBERO_COM_STATUS_SUCCESS;
}

void vinbero_core_registerExitHandler() {
    atexit(vinbero_core_exitHandler);
}

int vinbero_core_checkConfig(struct vinbero_com_Config* config, const char* moduleId) {
    VINBERO_COM_LOG_TRACE2();
    int ret;
    if((ret = vinbero_com_Config_check(config, moduleId)) < VINBERO_COM_STATUS_SUCCESS) {
        VINBERO_COM_LOG_ERROR("MODULE %s HAS WRONG CONFIG OR DOES NOT EXIST", moduleId);
        return ret;
    }

    struct vinbero_com_Object* childModuleIds;
    if((ret = vinbero_com_Config_getChildModuleIds(config, moduleId, &childModuleIds)) < VINBERO_COM_STATUS_SUCCESS)
        return ret;

    GENC_TREE_NODE_FOREACH(childModuleIds, index) {
        const struct vinbero_com_Object* childModuleId = GENC_TREE_NODE_RAW_GET(childModuleIds, index);
        if((ret = vinbero_core_checkConfig(config, VINBERO_COM_OBJECT_CONSTRING(childModuleId))) < VINBERO_COM_STATUS_SUCCESS)
            return ret;
    }
    return VINBERO_COM_STATUS_SUCCESS;
}

int vinbero_core_initLocalModule(struct vinbero_com_Module* module, struct vinbero_com_Config* config) {
    VINBERO_COM_LOG_TRACE2();
    GENC_TREE_NODE_SET_PARENT(module, NULL);
    module->id = "core";
    module->config = config;
    if(fastdl_open(&module->dlHandle, NULL, RTLD_LAZY | RTLD_GLOBAL) < 0) {
        VINBERO_COM_LOG_FATAL("FAILED TO OPEN CORE MODULE");
        return VINBERO_COM_ERROR_DLOPEN;
    }
    module->localModule.pointer = malloc(1 * sizeof(struct vinbero_core));
    struct vinbero_core* localModule = module->localModule.pointer;
    vinbero_com_Config_getInt(module->config, module, "vinbero.setUid", (int*)&localModule->setUid, (int)geteuid());
    vinbero_com_Config_getInt(module->config, module, "vinbero.setGid", (int*)&localModule->setGid, (int)getegid());
    return VINBERO_COM_STATUS_SUCCESS;
}

static int vinbero_core_loadModule(struct vinbero_com_Module* module) {
    int ret = VINBERO_COM_STATUS_SUCCESS;
    struct vinbero_com_Object* paths;

    VINBERO_COM_CONFIG_MGET_REQ(module->config, module, "paths", ARRAY, &paths);
    if(paths == NULL) {
        return VINBERO_COM_ERROR_INVALID_CONFIG;
    }
    if(GENC_TREE_NODE_SIZE(paths) < 1) {
        return VINBERO_COM_ERROR_INVALID_CONFIG;
    }
    GENC_TREE_NODE_FOREACH(paths, index) {
        struct vinbero_com_Object* path = GENC_TREE_NODE_RAW_GET(paths, index);
        if(!VINBERO_COM_OBJECT_IS_CONSTRING(path)) {
            return VINBERO_COM_ERROR_INVALID_CONFIG;
        }
        if(fastdl_open(&module->dlHandle, VINBERO_COM_OBJECT_CONSTRING(path), RTLD_LAZY | RTLD_GLOBAL) == 0) {
            module->path = VINBERO_COM_OBJECT_CONSTRING(path);
            return VINBERO_COM_STATUS_SUCCESS;
        }
        VINBERO_COM_LOG_DEBUG("FAILED TO OPEN %s FOR MODULE %s", VINBERO_COM_OBJECT_CONSTRING(path), module->id);
        ret = VINBERO_COM_ERROR_DLOPEN;
    }
    return ret;
}

struct vinbero_core_IfaceGroup {
    GENC_ALIST(const char);
};

struct vinbero_core_IfaceGroupAlist {
    GENC_ALIST(struct vinbero_core_IfaceGroup);
};

static bool vinbero_core_isIfaceBigger(const char* iface, const char* iface2) {
    return strncmp(iface, iface2, strlen(iface)) > 0;
}

static bool vinbero_core_ifaceGroupCompatible() {
}

static bool vinbero_core_getFirstCompatibleIfaceGroup() {
}

static bool vinbero_core_ifacesCompatible(const char* outIfacesOriginal, const char* inIfacesOriginal) {
    /* TODO: Support semicolon to separate iface groups */
    char* inIfaces = strdup(inIfacesOriginal);
    char* outIfaces = strdup(outIfacesOriginal);
    struct vinbero_core_IfaceGroupAlist outIfaceGroups;
    const char* ifaceGroup;
    GENC_ALIST_INIT(&outIfaceGroups);
    while((ifaceGroup = strsep(&outIfaces, ";")) != NULL) {
    }
    /*
    const char* iface;

    char* outIfaces2 = strdup(outIfaces);
    char* outIfaces3 = outIfaces2;
    struct vinbero_core_Ifaces outIfaceAlist;
    GENC_ALIST_INIT(&outIfaceAlist);
    while((iface = strsep(&outIfaces2, ",")) != NULL) {
        GENC_ALIST_PUSH(&outIfaceAlist, iface);
        GENC_ALIST_SORT(&outIfaceAlist, vinbero_core_isIfaceBigger);
    }

    char* inIfaces2 = strdup(inIfaces);
    char* inIfaces3 = inIfaces2;
    struct vinbero_core_Ifaces inIfaceAlist;
    GENC_ALIST_INIT(&inIfaceAlist);
    while((iface = strsep(&inIfaces2, ",")) != NULL) {
        GENC_ALIST_PUSH(&inIfaceAlist, iface);
        GENC_ALIST_SORT(&inIfaceAlist, vinbero_core_isIfaceBigger);
    }

    if(GENC_ALIST_SIZE(&outIfaceAlist) != GENC_ALIST_SIZE(&inIfaceAlist)) {
        GENC_ALIST_FREE(&outIfaceAlist);
        GENC_ALIST_FREE(&inIfaceAlist);
        free(outIfaces3);
        free(inIfaces3);
        return false;
    }
    GENC_ALIST_FOREACH(&outIfaceAlist, index) {
        if(strncmp(GENC_ALIST_RAW_GET(&outIfaceAlist, index), GENC_ALIST_RAW_GET(&inIfaceAlist, index), strlen(GENC_ALIST_RAW_GET(&outIfaceAlist, index))) != 0) {
            GENC_ALIST_FREE(&outIfaceAlist);
            GENC_ALIST_FREE(&inIfaceAlist);
            free(outIfaces3);
            free(inIfaces3);
            return false;
        }
    }

    GENC_ALIST_FREE(&outIfaceAlist);
    GENC_ALIST_FREE(&inIfaceAlist);
    free(outIfaces3);
    free(inIfaces3);
    */
    return true;
}

int vinbero_core_loadChildModules(struct vinbero_com_Module* module) {
    VINBERO_COM_LOG_TRACE2();
    int ret;
    struct vinbero_com_Object* childModuleIds;
    if((ret = vinbero_com_Config_getChildModuleIds(module->config, module->id, &childModuleIds)) < VINBERO_COM_STATUS_SUCCESS) {
        return ret;
    }

    GENC_TREE_NODE_FOREACH(childModuleIds, index) {
        struct vinbero_com_Module* childModule = malloc(sizeof(struct vinbero_com_Module));
        GENC_TREE_NODE_INIT(childModule);
        GENC_TREE_NODE_ADD(module, childModule);
        childModule->id = VINBERO_COM_OBJECT_CONSTRING(GENC_TREE_NODE_RAW_GET(childModuleIds, index));
        childModule->config = module->config;
        ret = vinbero_core_loadModule(childModule);
        if(ret == VINBERO_COM_ERROR_INVALID_CONFIG) {
            VINBERO_COM_LOG_ERROR("CONFIG FILE IS INVALID AT MODULE %s", childModule->id);
            return ret;
        }
        if(ret == VINBERO_COM_ERROR_DLOPEN) {
            VINBERO_COM_LOG_ERROR("DLOPEN ERROR ON MODULE %s: %s", childModule->id, fastdl_error()); // dlerror is not thread safe
            return ret;
        }
        VINBERO_COM_LOG_INFO("MODULE %s LOADED FROM %s", childModule->id, childModule->path);

        const char* outIfaces = NULL;
        VINBERO_COM_MODULE_META_GET(module, "OUT_IFACES", &outIfaces, &ret);
        if(ret < VINBERO_COM_STATUS_SUCCESS) {
            VINBERO_COM_LOG_ERROR("METADATA OUT_IFACES NOT FOUND ON MODULE %s", module->id);
            return VINBERO_COM_ERROR_INVALID_MODULE;
        }
        VINBERO_COM_LOG_DEBUG("OUT_IFACES OF MODULE %s IS %s", module->id, outIfaces);

        const char* inIfaces = NULL;
        VINBERO_COM_MODULE_META_GET(childModule, "IN_IFACES", &inIfaces, &ret);
        if(ret < VINBERO_COM_STATUS_SUCCESS) {
            VINBERO_COM_LOG_ERROR("METADATA IN_IFACES NOT FOUND ON MODULE %s", childModule->id);
            return VINBERO_COM_ERROR_INVALID_MODULE;
        }

        VINBERO_COM_LOG_DEBUG("IN_IFACES OF MODULE %s IS %s", childModule->id, inIfaces);

        if(vinbero_core_ifacesCompatible(outIfaces, inIfaces) == false) {
            VINBERO_COM_LOG_ERROR("MODULE %s AND %s ARE INCOMPATIBLE", module->id, childModule->id);
            return VINBERO_COM_ERROR_INVALID_MODULE;
        }

        if((ret = vinbero_core_loadChildModules(childModule)) < VINBERO_COM_STATUS_SUCCESS)
            return ret;

    }

    int minChildCount = 0;
    VINBERO_COM_MODULE_META_GET(module, "MIN_CHILD_COUNT", &minChildCount, &ret);
    if(ret < VINBERO_COM_STATUS_SUCCESS) {
        VINBERO_COM_LOG_ERROR("FAILED TO READ METADATA MIN_CHILD_COUNT");
        return VINBERO_COM_ERROR_INVALID_MODULE;
    }

    int maxChildCount = 0;
    VINBERO_COM_MODULE_META_GET(module, "MAX_CHILD_COUNT", &maxChildCount, &ret);
    if(ret < VINBERO_COM_STATUS_SUCCESS) {
        VINBERO_COM_LOG_ERROR("FAILED TO READ METADATA MAX_CHILD_COUNT");
        return VINBERO_COM_ERROR_INVALID_MODULE;
    }
    VINBERO_COM_LOG_DEBUG("MODULE %s HAS CHILD COUNT %d", module->id, GENC_TREE_NODE_SIZE(module));
    VINBERO_COM_LOG_DEBUG("MODULE %s HAS MIN_CHILD_COUNT %d AND MAX_CHILD_COUNT %d", module->id, minChildCount, maxChildCount);
    if(maxChildCount != -1 && (minChildCount > maxChildCount || minChildCount == -1)) {
        VINBERO_COM_LOG_ERROR("METADATA MIN_CHILD_COUND %d IS BIGGER THAN MAX_CHILD_COUNT %d ON MODULE %s", minChildCount, maxChildCount, module->id);
        return VINBERO_COM_ERROR_INVALID_MODULE;
    }
    if(minChildCount != -1 && minChildCount > GENC_TREE_NODE_SIZE(childModuleIds)) {
        VINBERO_COM_LOG_ERROR("CHILD COUNT %d IS SMALLER THAN MIN_CHILD_COUNT %d ON MODULE %s", GENC_TREE_NODE_SIZE(childModuleIds), minChildCount, module->id);
        return VINBERO_COM_ERROR_INVALID_CONFIG;
    }
    if(maxChildCount != -1 && maxChildCount < GENC_TREE_NODE_SIZE(childModuleIds)) {
        VINBERO_COM_LOG_ERROR("CHILD COUNT %d IS BIGGER THAN MAX_CHILD_COUNT %d ON MODULE %s", GENC_TREE_NODE_SIZE(childModuleIds), maxChildCount, module->id);
        return VINBERO_COM_ERROR_INVALID_CONFIG;
    }

    return VINBERO_COM_STATUS_SUCCESS;
}

int vinbero_core_initChildModules(struct vinbero_com_Module* module) {
    VINBERO_COM_LOG_TRACE2();
    int ret;
    GENC_TREE_NODE_FOREACH(module, index) {
        struct vinbero_com_Module* childModule = GENC_TREE_NODE_RAW_GET(module, index);
        VINBERO_COM_CALL(MODULE, init, childModule, &ret, childModule);
        if(ret < VINBERO_COM_STATUS_SUCCESS)
            return ret;
        if((ret = vinbero_core_initChildModules(childModule)) < 0)
            return ret;
    }
    return VINBERO_COM_STATUS_SUCCESS;
}

int vinbero_core_rInitChildModules(struct vinbero_com_Module* module) {
    VINBERO_COM_LOG_TRACE2();
    int ret;
    GENC_TREE_NODE_FOREACH(module, index) {
        struct vinbero_com_Module* childModule = GENC_TREE_NODE_RAW_GET(module, index);
        if((ret = vinbero_core_rInitChildModules(childModule)) < 0)
            return ret;

        VINBERO_COM_CALL(MODULE, rInit, childModule, &ret, childModule);
        if(ret < VINBERO_COM_STATUS_SUCCESS)
            return ret;
    }
    return VINBERO_COM_STATUS_SUCCESS;
}

static int vinbero_core_destroyChildModules(struct vinbero_com_Module* module) {
    VINBERO_COM_LOG_TRACE2();
    int ret;
    GENC_TREE_NODE_FOREACH(module, index) {
        struct vinbero_com_Module* childModule = GENC_TREE_NODE_RAW_GET(module, index);
        VINBERO_COM_CALL(MODULE, destroy, childModule, &ret, childModule);
        if(ret < VINBERO_COM_STATUS_SUCCESS)
            return ret;
        if((ret = vinbero_core_destroyChildModules(childModule)) < 0)
            return ret;
    }
    return VINBERO_COM_STATUS_SUCCESS;
}

static int vinbero_core_rDestroyChildModules(struct vinbero_com_Module* module) {
    VINBERO_COM_LOG_TRACE2();
    int ret;
    GENC_TREE_NODE_FOREACH(module, index) {
        struct vinbero_com_Module* childModule = GENC_TREE_NODE_RAW_GET(module, index);
        if((ret = vinbero_core_rDestroyChildModules(childModule)) < 0)
            return ret;
        VINBERO_COM_CALL(MODULE, rDestroy, childModule, &ret, childModule);
        if(ret < VINBERO_COM_STATUS_SUCCESS)
            return ret;
        GENC_TREE_NODE_FREE(childModule);
        free(childModule);
    }
    return VINBERO_COM_STATUS_SUCCESS;
}

int vinbero_core_setGid(struct vinbero_com_Module* module) {
    VINBERO_COM_LOG_TRACE2();
    struct vinbero_core* localModule = module->localModule.pointer;
    if(setgid(localModule->setGid) < 0) {
        VINBERO_COM_LOG_ERROR("FAILED TO CALL setgid()");
        return -errno;
    }
    return VINBERO_COM_STATUS_SUCCESS;
}

int vinbero_core_setUid(struct vinbero_com_Module* module) {
    VINBERO_COM_LOG_TRACE2();
    struct vinbero_core* localModule = module->localModule.pointer;
    if(setgid(localModule->setUid) < 0) {
        VINBERO_COM_LOG_ERROR("FAILED TO CALL setuid()");
        return -errno;
    }
    return VINBERO_COM_STATUS_SUCCESS;
}

int vinbero_core_start(struct vinbero_com_Module* module) {
    VINBERO_COM_LOG_TRACE2();
    int ret;
    jmp_buf* jumpBuffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jumpBuffer) == 0) {
        VINBERO_COM_LOG_TRACE2();
        pthread_key_create(&vinbero_core_tlKey, NULL);
        pthread_setspecific(vinbero_core_tlKey, jumpBuffer);
        GENC_TREE_NODE_FOREACH(module, index) {
            struct vinbero_com_Module* childModule = GENC_TREE_NODE_RAW_GET(module, index);
            VINBERO_COM_CALL(BASIC, service, childModule, &ret, childModule);
            if(ret < VINBERO_COM_STATUS_SUCCESS) {
                VINBERO_COM_LOG_ERROR("FAILED TO CALL vinbero_iface_BASIC_service()");
                break;
            }
        }
    }
    VINBERO_COM_LOG_TRACE2();
    free(jumpBuffer);
    pthread_setspecific(vinbero_core_tlKey, NULL);
    pthread_key_delete(vinbero_core_tlKey);
    vinbero_core_destroyChildModules(module);
    vinbero_core_rDestroyChildModules(module);
    free(module->localModule.pointer);
    return VINBERO_COM_STATUS_SUCCESS;
}
