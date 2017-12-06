#include <dlfcn.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgenc/genc_ArrayList.h>
#include "tucube_Core.h"

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
    size_t childModuleCount = GENC_ARRAY_LIST_SIZE(&childModuleIds);
    GENC_TREE_NODE_INIT_CHILDREN(module, childModuleCount);
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
        childModule->interface = malloc(1 * sizeof(struct tucube_Core_Interface)); // free() needed
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
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct tucube_Core_Interface* moduleInterface = childModule->interface;
        if((moduleInterface->tucube_ICore_service = dlsym(childModule->dlHandle, "tucube_ICore_service")) == NULL)
            errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_ICore_service()", __FILE__, __LINE__);
        if(moduleInterface->tucube_ICore_service(childModule, (void*[]){NULL}) == -1)
            errx(EXIT_FAILURE, "%s: %u: tucube_ICore_service() failed", __FILE__, __LINE__);
    }
//    dlclose(localModule->dlHandle);
    return 0;
}
