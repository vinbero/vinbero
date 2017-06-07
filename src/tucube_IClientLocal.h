#ifndef _TUCUBE_ICLIENT_LOCAL_H
#define _TUCUBE_ICLIENT_LOCAL_H

#include "tucube_Module.h"

#define TUCUBE_ICLIENT_LOCAL_FUNCTIONS                                                                           \
int tucube_IClientLocal_Init(struct tucube_Module* module, struct tucube_ClData_List* clDataList, void* args[]); \
int tucube_IClientLocal_service(struct tucube_Module* module, struct tucube_ClData* clData, void* args[]);       \
int tucube_IClientLocal_destroy(struct tucube_Module* module, struct tucube_ClData* clData)

#define TUCUBE_ICLIENT_LOCAL_FUNCTION_POINTERS                                                    \
int (*tucube_IClientLocal_init)(struct tucube_Module*, struct tucube_ClData_List*, void*[]);      \
int (*tucube_IClientLocal_service)(struct tucube_Module*, struct tucube_ClData*, void*[]); \
int (*tucube_IClientLocal_destroy)(struct tucube_Module*, struct tucube_ClData*)

#define TUCUBE_ICLIENT_LOCAL_DLSYM(module, modulePointerType)                \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IClientLocal_init);    \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IClientLocal_service); \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IClientLocal_destroy)

#endif
