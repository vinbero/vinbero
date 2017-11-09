#ifndef _TUCUBE_ICLOCAL_H
#define _TUCUBE_ICLOCAL_H

#include "tucube_Module.h"

#define TUCUBE_ICLOCAL_FUNCTIONS                                                                            \
int tucube_ICLocal_Init(struct tucube_Module* module, struct tucube_ClData_List* clDataList, void* args[]); \
int tucube_ICLocal_destroy(struct tucube_Module* module, struct tucube_ClData* clData)

#define TUCUBE_ICLOCAL_FUNCTION_POINTERS                                                    \
int (*tucube_ICLocal_init)(struct tucube_Module*, struct tucube_ClData_List*, void*[]);      \
int (*tucube_ICLocal_destroy)(struct tucube_Module*, struct tucube_ClData*)

#define TUCUBE_ICLOCAL_DLSYM(module, modulePointerType)                \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_ICLocal_init);    \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_ICLocal_destroy)

#endif
