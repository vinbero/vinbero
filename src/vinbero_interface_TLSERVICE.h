#ifndef _VINBERO_INTERFACE_TLSERVICE_H
#define _VINBERO_INTERFACE_TLSERVICE_H

#include <vinbero_common/vinbero_common_Module.h>

#define VINBERO_INTERFACE_TLSERVICE_FUNCTIONS \
int vinbero_interface_TLSERVICE_call(struct vinbero_common_Module* module, void* args[])

#define VINBERO_INTERFACE_TLSERVICE_FUNCTION_POINTERS \
int (*vinbero_interface_TLSERVICE_call)(struct vinbero_common_Module*, void*[])

struct vinbero_interface_TLSERVICE {
    VINBERO_INTERFACE_TLSERVICE_FUNCTION_POINTERS;
};

#define VINBERO_INTERFACE_TLSERVICE_DLSYM(interface, dlHandle, ret) \
do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_interface_TLSERVICE_call, ret); \
    if(*ret < 0) break; \
} while(0)

#endif
