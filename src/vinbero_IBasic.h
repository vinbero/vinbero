#ifndef _VINBERO_IBASIC_H
#define _VINBERO_IBASIC_H

#include "vinbero_Module.h"

#define VINBERO_IBASIC_FUNCTIONS \
int vinbero_IBasic_service(struct vinbero_Module* module, void* args[])

#define VINBERO_IBASIC_FUNCTION_POINTERS \
int (*vinbero_IBasic_service)(struct vinbero_Module*, void*[])

#define VINBERO_IBASIC_DLSYM(interface, dlHandle, errorVariable) \
do { \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IBasic_service, errorVariable); \
    if(*errorVariable == 1) break; \
} while(0)

#endif
