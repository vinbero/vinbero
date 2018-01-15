#ifndef _VINBERO_IMODULE_H
#define _VINBERO_IMODULE_H

#include "vinbero_Module.h"

#define VINBERO_IMODULE_FUNCTIONS                                                                    \
int vinbero_IModule_init(struct vinbero_Module* module, struct vinbero_Config* config, void* args[]);  \
int vinbero_IModule_rInit(struct vinbero_Module* module, struct vinbero_Config* config, void* args[]); \
int vinbero_IModule_destroy(struct vinbero_Module* module);                                           \
int vinbero_IModule_rDestroy(struct vinbero_Module* module)

#define VINBERO_IMODULE_FUNCTION_POINTERS                                            \
int (*vinbero_IModule_init)(struct vinbero_Module*, struct vinbero_Config*, void*[]);  \
int (*vinbero_IModule_rInit)(struct vinbero_Module*, struct vinbero_Config*, void*[]); \
int (*vinbero_IModule_destroy)(struct vinbero_Module*);                               \
int (*vinbero_IModule_rDestroy)(struct vinbero_Module*)

// DO WE NEED ARRAYS OF VOID POINTERS ANYWAY?

#endif
