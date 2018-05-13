#ifndef _VINBERO_ITLOCAL_H
#define _VINBERO_ITLOCAL_H

#include <vinbero_common/vinbero_common_Module.h>

#define VINBERO_ITLOCAL_FUNCTIONS                                                                    \
int vinbero_ITLocal_init(struct vinbero_common_Module* module, struct vinbero_common_Config* config, void* args[]);  \
int vinbero_ITLocal_rInit(struct vinbero_common_Module* module, struct vinbero_common_Config* config, void* args[]); \
int vinbero_ITLocal_destroy(struct vinbero_common_Module* module);                                           \
int vinbero_ITLocal_rDestroy(struct vinbero_common_Module* module)

#define VINBERO_ITLOCAL_FUNCTION_POINTERS                                            \
int (*vinbero_ITLocal_init)(struct vinbero_common_Module*, struct vinbero_common_Config*, void*[]);  \
int (*vinbero_ITLocal_rInit)(struct vinbero_common_Module*, struct vinbero_common_Config*, void*[]); \
int (*vinbero_ITLocal_destroy)(struct vinbero_common_Module*);                               \
int (*vinbero_ITLocal_rDestroy)(struct vinbero_common_Module*)

struct vinbero_ITLocal_Interface {
    VINBERO_ITLOCAL_FUNCTION_POINTERS;
};

#define VINBERO_ITLOCAL_DLSYM(interface, dlHandle, ret)                      \
do {                                                                                  \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_ITLocal_init, ret);     \
    if(*ret < 0) break;                                                    \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_ITLocal_rInit, ret);    \
    if(*ret < 0) break;                                                    \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_ITLocal_destroy, ret);  \
    if(*ret < 0) break;                                                    \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_ITLocal_rDestroy, ret); \
    if(*ret < 0) break;                                                    \
} while(0)

#endif
