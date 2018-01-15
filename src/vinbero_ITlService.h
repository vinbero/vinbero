#ifndef _VINBERO_ITLSERVICE_H
#define _VINBERO_ITLSERVICE_H

#include <pthread.h>
#include "vinbero_Module.h"

#define VINBERO_ITLSERVICE_FUNCTIONS \
int vinbero_ITlService_call(struct vinbero_Module* module, void* args[])

#define VINBERO_ITLSERVICE_FUNCTION_POINTERS \
int (*vinbero_ITlService_call)(struct vinbero_Module*, void*[])

#define VINBERO_ITLSERVICE_DLSYM(interface, dlHandle, errorVariable)                  \
do {                                                                                 \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_ITlService_call, errorVariable); \
    if(*errorVariable == 1) break;                                                   \
} while(0)

#endif
