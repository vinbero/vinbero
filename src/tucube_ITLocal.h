#ifndef _TUCUBE_ITLOCAL_H
#define _TUCUBE_ITLOCAL_H

#include "tucube_Module.h"

#define TUCUBE_ITLOCAL_FUNCTIONS                                                                     \
int tucube_ITLocal_tlInit(struct tucube_Module* module, struct tucube_Config* config, void* args[]); \
int tucube_ITLocal_tlDestroy(struct tucube_Module* module)

#define TUCUBE_ITLOCAL_FUNCTION_POINTERS                                             \
int (*tucube_ITLocal_tlInit)(struct tucube_Module*, struct tucube_Config*, void*[]); \
int (*tucube_ITLocal_tlDestroy)(struct tucube_Module*)

#endif
