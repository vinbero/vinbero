#ifndef _VINBERO_INTERFACE_TLOCAL_H
#define _VINBERO_INTERFACE_TLOCAL_H

#include <vinbero_common/vinbero_common_Module.h>

#define VINBERO_INTERFACE_TLOCAL_FUNCTIONS \
int vinbero_Interface_TLOCAL_init(struct vinbero_common_Module* module); \
int vinbero_Interface_TLOCAL_rInit(struct vinbero_common_Module* module); \
int vinbero_Interface_TLOCAL_sendArgs(struct vinbero_common_Module* module, void* args[]); \
int vinbero_Interface_TLOCAL_recvArgs(struct vinbero_common_Module* module, void* args[]); \
int vinbero_Interface_TLOCAL_destroy(struct vinbero_common_Module* module); \
int vinbero_Interface_TLOCAL_rDestroy(struct vinbero_common_Module* module)

#define VINBERO_INTERFACE_TLOCAL_FUNCTION_POINTERS \
int (*vinbero_Interface_TLOCAL_init)(struct vinbero_common_Module*); \
int (*vinbero_Interface_TLOCAL_rInit)(struct vinbero_common_Module*); \
int (*vinbero_Interface_TLOCAL_sendArgs)(struct vinbero_common_Module*, void*[]); \
int (*vinbero_Interface_TLOCAL_recvArgs)(struct vinbero_common_Module*, void*[]); \
int (*vinbero_Interface_TLOCAL_destroy)(struct vinbero_common_Module*); \
int (*vinbero_Interface_TLOCAL_rDestroy)(struct vinbero_common_Module*)

struct vinbero_Interface_TLOCAL {
    VINBERO_INTERFACE_TLOCAL_FUNCTION_POINTERS;
};

#define VINBERO_INTERFACE_TLOCAL_DLSYM(interface, dlHandle, ret) \
do { \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_Interface_TLOCAL_init, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_Interface_TLOCAL_rInit, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_Interface_TLOCAL_destroy, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_Interface_TLOCAL_rDestroy, ret); \
    if(*ret < 0) break; \
} while(0)

#endif
