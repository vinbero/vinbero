#ifndef _VINBERO_INTERFACE_MODULE_H
#define _VINBERO_INTERFACE_MODULE_H

#include <vinbero_common/vinbero_common_Config.h>
#include <vinbero_common/vinbero_common_Module.h>

#define VINBERO_INTERFACE_MODULE_FUNCTIONS \
int vinbero_interface_MODULE_init(struct vinbero_common_Module* module); \
int vinbero_interface_MODULE_rInit(struct vinbero_common_Module* module); \
int vinbero_interface_MODULE_destroy(struct vinbero_common_Module* module); \
int vinbero_interface_MODULE_rDestroy(struct vinbero_common_Module* module)

#define VINBERO_INTERFACE_MODULE_FUNCTION_POINTERS \
int (*vinbero_interface_MODULE_init)(struct vinbero_common_Module*); \
int (*vinbero_interface_MODULE_rInit)(struct vinbero_common_Module*); \
int (*vinbero_interface_MODULE_destroy)(struct vinbero_common_Module*); \
int (*vinbero_interface_MODULE_rDestroy)(struct vinbero_common_Module*)

struct vinbero_interface_MODULE {
    VINBERO_INTERFACE_MODULE_FUNCTION_POINTERS;
};

#define VINBERO_INTERFACE_MODULE_DLSYM(interface, dlHandle, ret) do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_interface_MODULE_init, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_interface_MODULE_rInit, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_interface_MODULE_destroy, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_interface_MODULE_rDestroy, ret); \
    if(*ret < 0) break; \
} while(0)

#endif
