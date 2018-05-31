#ifndef _VINBERO_INTERFACE_CLSERVICE_H
#define _VINBERO_INTERFACE_CLSERVICE_H

#include <vinbero_common/vinbero_common_Module.h>
#include <vinbero_common/vinbero_common_ClData.h>

#define VINBERO_INTERFACE_CLSERVICE_FUNCTIONS \
int vinbero_Interface_CLSERVICE_call(struct vinbero_common_Module* module, struct vinbero_common_ClData* clData, void* args[])

#define VINBERO_INTERFACE_CLSERVICE_FUNCTION_POINTERS \
int (*vinbero_Interface_CLSERVICE_call)(struct vinbero_common_Module*, struct vinbero_common_ClData*, void*[])

struct vinbero_Interface_CLSERVICE {
    VINBERO_INTERFACE_CLSERVICE_FUNCTION_POINTERS;
};

#define VINBERO_INTERFACE_CLSERVICE_DLSYM(interface, dlHandle, ret) \
do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_Interface_CLSERVICE_call, ret); \
    if(*ret < 0) break; \
} while(0);

#endif
