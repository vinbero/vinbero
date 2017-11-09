#ifndef _TUCUBE_IBASE_H
#define _TUCUBE_IBASE_H

#include "tucube_Module.h"

#define TUCUBE_IBASE_FUNCTIONS                                                                            \
int tucube_IBase_init(struct tucube_Config* config, struct tucube_Module* module, void* args[]); \
int tucube_IBase_tlInit(struct tucube_Module* module, struct tucube_Config* config, void* args[]);        \
int tucube_IBase_tlDestroy(struct tucube_Module* module);                                                 \
int tucube_IBase_destroy(struct tucube_Module* module)

#define TUCUBE_IBASE_FUNCTION_POINTERS                                                \
int (*tucube_IBase_init)(struct tucube_Config*, struct tucube_Module*, void*[]); \
int (*tucube_IBase_destroy)(struct tucube_Module*);                                   \
int (*tucube_IBase_tlInit)(struct tucube_Module*, struct tucube_Config*, void*[]);    \
int (*tucube_IBase_tlDestroy)(struct tucube_Module*)

#define TUCUBE_IBASE_DLSYM(module, modulePointerType)                 \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IBase_init);    \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IBase_destroy); \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IBase_tlInit);  \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IBase_tlDestroy)

#endif
