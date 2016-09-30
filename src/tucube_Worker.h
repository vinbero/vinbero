#ifndef _TUCUBE_WORKER_H
#define _TUCUBE_WORKER_H

#include <pthread.h>
#include <stdbool.h>
#include "config.h"
#include "tucube_Module.h"

struct tucube_Worker_Args {
    bool exit;
    pthread_mutex_t* exitMutex;
    pthread_cond_t* exitCond;
    int serverSocket;
    pthread_mutex_t* serverSocketMutex;
    struct tucube_Module* module;
    struct tucube_Module_Args* moduleArgs;
    int (*tucube_Module_tlInit)(struct tucube_Module*, struct tucube_Module_Args*);
    int (*tucube_Module_start)(struct tucube_Module*, int*, pthread_mutex_t*);
    int (*tucube_Module_tlDestroy)(struct tucube_Module*);
};

void* tucube_Worker_start(void* workerArgs);

#endif
