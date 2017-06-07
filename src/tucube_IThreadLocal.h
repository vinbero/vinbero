#ifndef _TUCUBE_ITHREAD_LOCAL_H
#define _TUCUBE_ITHREAD_LOCAL_H

#include <pthread.h>
#include "tucube_Module.h"

#define TUCUBE_ITHREAD_LOCAL_FUNCTIONS \
int tucube_IThreadLocal_service(struct tucube_Module* module, void* args[])

#define TUCUBE_ITHREAD_LOCAL_FUNCTION_POINTERS \
int (*tucube_IThreadLocal_service)(struct tucube_Module*, void*[])

#define TUCUBE_ITHREAD_LOCAL_DLSYM(module, modulePointerType) \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IThreadLocal_service)

#endif
