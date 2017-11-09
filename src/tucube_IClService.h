#ifndef _TUCUBE_ICLSERVICE_H
#define _TUCUBE_ICLSERVICE_H

#include "tucube_Module.h"
#include "tucube_ClData.h"

#define TUCUBE_ICLSERVICE_FUNCTIONS \
int tucube_IClService_call(struct tucube_Module* module, struct tucube_ClData* clData, void* args[])

#define TUCUBE_ICLSERVICE_FUNCTION_POINTERS \
int (*tucube_IClService_call)(struct tucube_Module*, struct tucube_ClData*, void*[])

#define TUCUBE_ICLSERVICE_DLSYM(module, modulePointerType) \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IClService_call)

#endif
