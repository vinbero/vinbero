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

#endif
