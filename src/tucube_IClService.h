#ifndef _TUCUBE_ICLSERVICE_H
#define _TUCUBE_ICLSERVICE_H

#include "tucube_Module.h"

#define TUCUBE_ICLSERVICE_FUNCTIONS                                                                           \
int tucube_IClService_Init(struct tucube_Module* module, struct tucube_ClData_List* clDataList, void* args[]); \
int tucube_IClService_call(struct tucube_Module* module, struct tucube_ClData* clData, void* args[]);       \
int tucube_IClService_destroy(struct tucube_Module* module, struct tucube_ClData* clData)

#define TUCUBE_ICLSERVICE_FUNCTION_POINTERS                                                    \
int (*tucube_IClService_init)(struct tucube_Module*, struct tucube_ClData_List*, void*[]);      \
int (*tucube_IClService_call)(struct tucube_Module*, struct tucube_ClData*, void*[]); \
int (*tucube_IClService_destroy)(struct tucube_Module*, struct tucube_ClData*)

#define TUCUBE_ICLSERVICE_DLSYM(module, modulePointerType)                \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IClService_init);    \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IClService_call); \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IClService_destroy)

#endif
