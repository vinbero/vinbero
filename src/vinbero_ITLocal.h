#ifndef _VINBERO_ITLOCAL_H
#define _VINBERO_ITLOCAL_H

#include "vinbero_Module.h"

#define VINBERO_ITLOCAL_FUNCTIONS                                                                    \
int vinbero_ITLocal_init(struct vinbero_Module* module, struct vinbero_Config* config, void* args[]);  \
int vinbero_ITLocal_rInit(struct vinbero_Module* module, struct vinbero_Config* config, void* args[]); \
int vinbero_ITLocal_destroy(struct vinbero_Module* module);                                           \
int vinbero_ITLocal_rDestroy(struct vinbero_Module* module)

#define VINBERO_ITLOCAL_FUNCTION_POINTERS                                            \
int (*vinbero_ITLocal_init)(struct vinbero_Module*, struct vinbero_Config*, void*[]);  \
int (*vinbero_ITLocal_rInit)(struct vinbero_Module*, struct vinbero_Config*, void*[]); \
int (*vinbero_ITLocal_destroy)(struct vinbero_Module*);                               \
int (*vinbero_ITLocal_rDestroy)(struct vinbero_Module*)

#define VINBERO_ITLOCAL_DLSYM(interface, dlHandle, errorVariable)                      \
do {                                                                                  \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_ITLocal_init, errorVariable);     \
    if(*errorVariable == 1) break;                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_ITLocal_rInit, errorVariable);    \
    if(*errorVariable == 1) break;                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_ITLocal_destroy, errorVariable);  \
    if(*errorVariable == 1) break;                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_ITLocal_rDestroy, errorVariable); \
    if(*errorVariable == 1) break;                                                    \
} while(0)

/*
#define VINBERO_ITLOCAL_INIT_CHILDREN(module, childInterfaceType, config, args) \
GENC_TREE_NODE_FOR_EACH_CHILD(module, index) { \
    struct vinbero_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index) \
    childInterfaceType* childInterface = childModule->interface; \
    if(childInterface->vinbero_ITLocal_init(childModule, config, args) == -1) \
        *errorVariable = 1; \
}
*/

#endif
