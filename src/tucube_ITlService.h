#ifndef _TUCUBE_ITL_SERVICE_H
#define _TUCUBE_ITL_SERVICE_H

#include <pthread.h>
#include "tucube_Module.h"

#define TUCUBE_ITL_SERVICE_FUNCTIONS \
int tucube_ITlService_call(struct tucube_Module* module, void* args[])

#define TUCUBE_ITL_SERVICE_FUNCTION_POINTERS \
int (*tucube_ITlService_call)(struct tucube_Module*, void*[])

#define TUCUBE_ITL_SERVICE_DLSYM(module, modulePointerType) \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_ITlService_call)

#endif
