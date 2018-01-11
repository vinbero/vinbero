#ifndef _TUCUBE_ICLOCAL_H
#define _TUCUBE_ICLOCAL_H

#include "tucube_Module.h"

#define TUCUBE_ICLOCAL_FUNCTIONS                                                                   \
int tucube_ICLocal_init(struct tucube_Module* module, struct tucube_ClData* clData, void* args[]); \
int tucube_ICLocal_destroy(struct tucube_Module* module, struct tucube_ClData* clData)

#define TUCUBE_ICLOCAL_FUNCTION_POINTERS                                           \
int (*tucube_ICLocal_init)(struct tucube_Module*, struct tucube_ClData*, void*[]); \
int (*tucube_ICLocal_destroy)(struct tucube_Module*, struct tucube_ClData*)

#define TUCUBE_ICLOCAL_DLSYM(interface, dlHandle, errorVariable)                     \
do {                                                                                 \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_ICLocal_init, errorVariable);    \
    if(*errorVariable == 1) break;                                                   \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_ICLocal_destroy, errorVariable); \
    if(*errorVariable == 1) break;                                                   \
} while(0)

#endif
