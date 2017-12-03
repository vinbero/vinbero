#ifndef _TUCUBE_IMODULE_H
#define _TUCUBE_IMODULE_H

#include "tucube_Module.h"

#define TUCUBE_IMODULE_FUNCTIONS                                                                     \
int tucube_IModule_init(struct tucube_Module* module, struct tucube_Config* config, void* args[]);   \
int tucube_IModule_destroy(struct tucube_Module* module);                                            \
int tucube_IModule_tlInit(struct tucube_Module* module, struct tucube_Config* config, void* args[]); \
int tucube_IModule_tlDestroy(struct tucube_Module* module)

#define TUCUBE_IMODULE_FUNCTION_POINTERS                                             \
int (*tucube_IModule_init)(struct tucube_Module*, struct tucube_Config*, void*[]);   \
int (*tucube_IModule_destroy)(struct tucube_Module*);                                \
int (*tucube_IModule_tlInit)(struct tucube_Module*, struct tucube_Config*, void*[]); \
int (*tucube_IModule_tlDestroy)(struct tucube_Module*)

#endif
