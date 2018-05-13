#ifndef _VINBERO_ICLOCAL_H
#define _VINBERO_ICLOCAL_H

#include <vinbero_common/vinbero_common_Module.h>

#define VINBERO_ICLOCAL_FUNCTIONS \
int vinbero_ICLocal_init(struct vinbero_common_Module* module, struct vinbero_ClData* clData, void* args[]); \
int vinbero_ICLocal_destroy(struct vinbero_common_Module* module, struct vinbero_ClData* clData)

#define VINBERO_ICLOCAL_FUNCTION_POINTERS \
int (*vinbero_ICLocal_init)(struct vinbero_common_Module*, struct vinbero_ClData*, void*[]); \
int (*vinbero_ICLocal_destroy)(struct vinbero_common_Module*, struct vinbero_ClData*)

struct vinbero_ICLocal_Interface {
    VINBERO_ICLOCAL_FUNCTION_POINTERS;
};

#define VINBERO_ICLOCAL_DLSYM(interface, dlHandle, ret) do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_ICLocal_init, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_ICLocal_destroy, ret); \
    if(*ret < 0) break; \
} while(0)

#endif
