#ifndef _VINBERO_IBASIC_H
#define _VINBERO_IBASIC_H

#include <vinbero_common/vinbero_common_Module.h>

#define VINBERO_IBASIC_FUNCTIONS \
int vinbero_IBasic_service(struct vinbero_common_Module* module, void* args[])

#define VINBERO_IBASIC_FUNCTION_POINTERS \
int (*vinbero_IBasic_service)(struct vinbero_common_Module*, void*[])

struct vinbero_IBasic_Interface {
    VINBERO_IBASIC_FUNCTION_POINTERS;
};

#define VINBERO_IBASIC_DLSYM(interface, dlHandle, ret) \
do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_IBasic_service, ret); \
    if(*ret < 0) break; \
} while(0)

#endif
