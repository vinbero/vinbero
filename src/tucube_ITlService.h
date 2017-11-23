#ifndef _TUCUBE_ITLSERVICE_H
#define _TUCUBE_ITLSERVICE_H

#include <pthread.h>
#include "tucube_Module.h"

#define TUCUBE_ITLSERVICE_FUNCTIONS \
int tucube_ITlService_call(struct tucube_Module* module, void* args[])

#define TUCUBE_ITLSERVICE_FUNCTION_POINTERS \
int (*tucube_ITlService_call)(struct tucube_Module*, void*[])

#define TUCUBE_ITLSERVICE_DLSYM(module, localModuleType) \
TUCUBE_MODULE_DLSYM(module, localModuleType, tucube_ITlService_call)

#endif
