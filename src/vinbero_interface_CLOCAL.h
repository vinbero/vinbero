#ifndef _VINBERO_INTERFACE_CLOCAL_H
#define _VINBERO_INTERFACE_CLOCAL_H

#include <vinbero_common/vinbero_common_ClModule.h>

#define VINBERO_INTERFACE_CLOCAL_FUNCTIONS \
int vinbero_interface_CLOCAL_init(struct vinbero_common_ClModule* clModule); \
int vinbero_interface_CLOCAL_rInit(struct vinbero_common_ClModule* clModule); \
int vinbero_interface_CLOCAL_destroy(struct vinbero_common_ClModule* clModule); \
int vinbero_interface_CLOCAL_rDestroy(struct vinbero_common_ClModule* clModule)

#define VINBERO_INTERFACE_CLOCAL_FUNCTION_POINTERS \
int (*vinbero_interface_CLOCAL_init)(struct vinbero_common_ClModule*); \
int (*vinbero_interface_CLOCAL_rInit)(struct vinbero_common_ClModule*); \
int (*vinbero_interface_CLOCAL_destroy)(struct vinbero_common_ClModule*); \
int (*vinbero_interface_CLOCAL_rDestroy)(struct vinbero_common_ClModule*)

struct vinbero_interface_CLOCAL {
    VINBERO_INTERFACE_CLOCAL_FUNCTION_POINTERS;
};

#define VINBERO_INTERFACE_CLOCAL_DLSYM(interface, dlHandle, ret) do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_interface_CLOCAL_init, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_interface_CLOCAL_rInit, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_interface_CLOCAL_destroy, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_interface_CLOCAL_rDestroy, ret); \
    if(*ret < 0) break; \
} while(0)

#endif
