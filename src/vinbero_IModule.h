#ifndef _VINBERO_IMODULE_H
#define _VINBERO_IMODULE_H

#include <vinbero_common/vinbero_common_Config.h>
#include <vinbero_common/vinbero_common_Module.h>

#define VINBERO_IMODULE_FUNCTIONS \
int vinbero_IModule_init(struct vinbero_common_Module* module, struct vinbero_common_Config* config, void* args[]); \
int vinbero_IModule_rInit(struct vinbero_common_Module* module, struct vinbero_common_Config* config, void* args[]); \
int vinbero_IModule_destroy(struct vinbero_common_Module* module); \
int vinbero_IModule_rDestroy(struct vinbero_common_Module* module)

#define VINBERO_IMODULE_FUNCTION_POINTERS \
int (*vinbero_IModule_init)(struct vinbero_common_Module*, struct vinbero_common_Config*, void*[]); \
int (*vinbero_IModule_rInit)(struct vinbero_common_Module*, struct vinbero_common_Config*, void*[]); \
int (*vinbero_IModule_destroy)(struct vinbero_common_Module*); \
int (*vinbero_IModule_rDestroy)(struct vinbero_common_Module*)

struct vinbero_IModule_Interface {
    VINBERO_IMODULE_FUNCTION_POINTERS;
};

#define VINBERO_IMODULE_DLSYM(interface, dlHandle, ret) do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_IModule_init, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_IModule_rInit, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_IModule_destroy, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_IModule_rDestroy, ret); \
    if(*ret < 0) break; \
} while(0)

#endif
