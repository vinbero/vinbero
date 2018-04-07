#ifndef _VINBERO_ICLOCAL_H
#define _VINBERO_ICLOCAL_H

#include "vinbero_Module.h"

#define VINBERO_ICLOCAL_FUNCTIONS \
int vinbero_ICLocal_init(struct vinbero_Module* module, struct vinbero_ClData* clData, void* args[]); \
int vinbero_ICLocal_destroy(struct vinbero_Module* module, struct vinbero_ClData* clData)

#define VINBERO_ICLOCAL_FUNCTION_POINTERS \
int (*vinbero_ICLocal_init)(struct vinbero_Module*, struct vinbero_ClData*, void*[]); \
int (*vinbero_ICLocal_destroy)(struct vinbero_Module*, struct vinbero_ClData*)

#define VINBERO_ICLOCAL_DLSYM(interface, dlHandle, errorVariable) do { \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_ICLocal_init, errorVariable); \
    if(*errorVariable == 1) break; \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_ICLocal_destroy, errorVariable); \
    if(*errorVariable == 1) break; \
} while(0)

#endif
