#ifndef _VINBERO_IMODULE_H
#define _VINBERO_IMODULE_H

#include <vinbero_common/vinbero_common_Config.h>
#include <vinbero_common/vinbero_common_Module.h>

#define VINBERO_IMODULE_FUNCTIONS \
int vinbero_IMODULE_init(struct vinbero_common_Module* module, struct vinbero_common_Config* config, void* args[]); \
int vinbero_IMODULE_rInit(struct vinbero_common_Module* module, struct vinbero_common_Config* config, void* args[]); \
int vinbero_IMODULE_destroy(struct vinbero_common_Module* module); \
int vinbero_IMODULE_rDestroy(struct vinbero_common_Module* module)

#define VINBERO_IMODULE_FUNCTION_POINTERS \
int (*vinbero_IMODULE_init)(struct vinbero_common_Module*, struct vinbero_common_Config*, void*[]); \
int (*vinbero_IMODULE_rInit)(struct vinbero_common_Module*, struct vinbero_common_Config*, void*[]); \
int (*vinbero_IMODULE_destroy)(struct vinbero_common_Module*); \
int (*vinbero_IMODULE_rDestroy)(struct vinbero_common_Module*)

struct vinbero_IMODULE_Interface {
    VINBERO_IMODULE_FUNCTION_POINTERS;
};

#define VINBERO_IMODULE_DLSYM(interface, dlHandle, ret) do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_IMODULE_init, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_IMODULE_rInit, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_IMODULE_destroy, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_IMODULE_rDestroy, ret); \
    if(*ret < 0) break; \
} while(0)

#endif
