#ifndef _TUCUBE_ICORE_H
#define _TUCUBE_ICORE_H

#include "tucube_Module.h"

#define TUCUBE_ICORE_FUNCTIONS \
int tucube_ICore_service(struct tucube_Module* module, void* args[])

#define TUCUBE_ICORE_FUNCTION_POINTERS \
int (*tucube_ICore_service)(struct tucube_Module*, void*[])

#define TUCUBE_ICORE_DLSYM(module, localModuleType) \
TUCUBE_MODULE_DLSYM(module, localModuleType, tucube_ICore_service)

#endif
