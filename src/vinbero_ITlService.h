#ifndef _VINBERO_ITLSERVICE_H
#define _VINBERO_ITLSERVICE_H

#include <pthread.h>
#include <vinbero_common/vinbero_common_Module.h>

#define VINBERO_ITLSERVICE_FUNCTIONS \
int vinbero_ITlService_call(struct vinbero_common_Module* module, void* args[])

#define VINBERO_ITLSERVICE_FUNCTION_POINTERS \
int (*vinbero_ITlService_call)(struct vinbero_common_Module*, void*[])

#define VINBERO_ITLSERVICE_DLSYM(interface, dlHandle, ret)                  \
do {                                                                                 \
    VINBERO_COMMON_MODULE_DLSYM(interface, dlHandle, vinbero_ITlService_call, ret); \
    if(*ret < 0) break;                                                   \
} while(0)

#endif
