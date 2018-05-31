#ifndef _VINBERO_ICLSERVICE_H
#define _VINBERO_ICLSERVICE_H

#include <vinbero_common/vinbero_common_Module.h>
#include <vinbero_common/vinbero_common_ClData.h>

#define VINBERO_ICLSERVICE_FUNCTIONS \
int vinbero_ICLSERVICE_call(struct vinbero_common_Module* module, struct vinbero_common_ClData* clData, void* args[])

#define VINBERO_ICLSERVICE_FUNCTION_POINTERS \
int (*vinbero_ICLSERVICE_call)(struct vinbero_common_Module*, struct vinbero_common_ClData*, void*[])

struct vinbero_ICLSERVICE_Interface {
    VINBERO_ICLSERVICE_FUNCTION_POINTERS;
};

#define VINBERO_ICLSERVICE_DLSYM(interface, dlHandle, ret) \
do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_ICLSERVICE_call, ret); \
    if(*ret < 0) break; \
} while(0);

#endif
