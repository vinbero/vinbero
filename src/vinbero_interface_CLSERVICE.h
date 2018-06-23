#ifndef _VINBERO_INTERFACE_CLSERVICE_H
#define _VINBERO_INTERFACE_CLSERVICE_H

#include <vinbero_common/vinbero_common_ClModule.h>

#define VINBERO_INTERFACE_CLSERVICE_FUNCTIONS \
int vinbero_interface_CLSERVICE_call(struct vinbero_common_ClModule* clModule)

#define VINBERO_INTERFACE_CLSERVICE_FUNCTION_POINTERS \
int (*vinbero_interface_CLSERVICE_call)(struct vinbero_common_ClModule*)

struct vinbero_interface_CLSERVICE {
    VINBERO_INTERFACE_CLSERVICE_FUNCTION_POINTERS;
};

#define VINBERO_INTERFACE_CLSERVICE_DLSYM(interface, dlHandle, ret) \
do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_interface_CLSERVICE_call, ret); \
    if(*ret < 0) break; \
} while(0);

#endif
