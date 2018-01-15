#ifndef _TUCUBE_ITLOCAL_H
#define _TUCUBE_ITLOCAL_H

#include "tucube_Module.h"

#define TUCUBE_ITLOCAL_FUNCTIONS                                                                    \
int tucube_ITLocal_init(struct tucube_Module* module, struct tucube_Config* config, void* args[]);  \
int tucube_ITLocal_rInit(struct tucube_Module* module, struct tucube_Config* config, void* args[]); \
int tucube_ITLocal_destroy(struct tucube_Module* module);                                           \
int tucube_ITLocal_rDestroy(struct tucube_Module* module)

#define TUCUBE_ITLOCAL_FUNCTION_POINTERS                                            \
int (*tucube_ITLocal_init)(struct tucube_Module*, struct tucube_Config*, void*[]);  \
int (*tucube_ITLocal_rInit)(struct tucube_Module*, struct tucube_Config*, void*[]); \
int (*tucube_ITLocal_destroy)(struct tucube_Module*);                               \
int (*tucube_ITLocal_rDestroy)(struct tucube_Module*)

#define TUCUBE_ITLOCAL_DLSYM(interface, dlHandle, errorVariable)                      \
do {                                                                                  \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_ITLocal_init, errorVariable);     \
    if(*errorVariable == 1) break;                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_ITLocal_rInit, errorVariable);    \
    if(*errorVariable == 1) break;                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_ITLocal_destroy, errorVariable);  \
    if(*errorVariable == 1) break;                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_ITLocal_rDestroy, errorVariable); \
    if(*errorVariable == 1) break;                                                    \
} while(0)

/*
#define TUCUBE_ITLOCAL_INIT_CHILDREN(module, childInterfaceType, config, args) \
GENC_TREE_NODE_FOR_EACH_CHILD(module, index) { \
    struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index) \
    childInterfaceType* childInterface = childModule->interface; \
    if(childInterface->tucube_ITLocal_init(childModule, config, args) == -1) \
        *errorVariable = 1; \
}
*/

#endif
