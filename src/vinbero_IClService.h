#ifndef _VINBERO_ICLSERVICE_H
#define _VINBERO_ICLSERVICE_H

#include "vinbero_Module.h"
#include "vinbero_ClData.h"

#define VINBERO_ICLSERVICE_FUNCTIONS \
int vinbero_IClService_call(struct vinbero_Module* module, struct vinbero_ClData* clData, void* args[])

#define VINBERO_ICLSERVICE_FUNCTION_POINTERS \
int (*vinbero_IClService_call)(struct vinbero_Module*, struct vinbero_ClData*, void*[])

struct vinbero_IClService_Interface {
    VINBER_ICLSERVICE_FUNCITON_POINTERS;
};

#define VINBERO_ICLSERVICE_DLSYM(interface, dlHandle, errorVariable) \
do { \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IClService_call, errorVariable); \
    if(*errorVariable == 1) break; \
} while(0);

#endif
