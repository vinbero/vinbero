#ifndef _VINBERO_IMODULE_H
#define _VINBERO_IMODULE_H

#include "vinbero_Config.h"
#include "vinbero_Module.h"

#define VINBERO_IMODULE_FUNCTIONS \
int vinbero_IModule_init(struct vinbero_Module* module, struct vinbero_Config* config, void* args[]); \
int vinbero_IModule_rInit(struct vinbero_Module* module, struct vinbero_Config* config, void* args[]); \
int vinbero_IModule_destroy(struct vinbero_Module* module); \
int vinbero_IModule_rDestroy(struct vinbero_Module* module)

#define VINBERO_IMODULE_FUNCTION_POINTERS \
int (*vinbero_IModule_init)(struct vinbero_Module*, struct vinbero_Config*, void*[]); \
int (*vinbero_IModule_rInit)(struct vinbero_Module*, struct vinbero_Config*, void*[]); \
int (*vinbero_IModule_destroy)(struct vinbero_Module*); \
int (*vinbero_IModule_rDestroy)(struct vinbero_Module*)

struct vinbero_IModule_Interface {
    VINBERO_IMODULE_FUNCTION_POINTERS;
};

#define VINBERO_IMODULE_DLSYM(interface, dlHandle, errorVariable) do { \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IModule_init, errorVariable); \
    if(*errorVariable == 1) break; \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IModule_rInit, errorVariable); \
    if(*errorVariable == 1) break; \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IModule_destroy, errorVariable); \
    if(*errorVariable == 1) break; \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IModule_rDestroy, errorVariable); \
    if(*errorVariable == 1) break; \
} while(0)

#endif
