#ifndef _TUCUBE_ICORE_H
#define _TUCUBE_ICORE_H

#include <pthread.h>
#include "tucube_Module.h"

#define TUCUBE_ICORE_FUNCTIONS \
int tucube_ICore_service(struct tucube_Module* module, int* serverSocket, pthread_mutex_t* serverSocketMutex)

#define TUCUBE_ICORE_FUNCTION_POINTERS \
int (*tucube_ICore_service)(struct tucube_Module*, int*, pthread_mutex_t*)

#endif
