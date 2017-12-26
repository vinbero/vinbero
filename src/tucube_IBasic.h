#ifndef _TUCUBE_IBASIC_H
#define _TUCUBE_IBASIC_H

#include "tucube_Module.h"

#define TUCUBE_IBASIC_FUNCTIONS \
int tucube_IBasic_service(struct tucube_Module* module, void* args[])

#define TUCUBE_IBASIC_FUNCTION_POINTERS \
int (*tucube_IBasic_service)(struct tucube_Module*, void*[])

#define TUCUBE_IBASIC_DLSYM(module, localModuleType) \
TUCUBE_MODULE_DLSYM(module, localModuleType, tucube_IBasic_service)

#endif
