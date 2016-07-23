#ifndef _GONM_WORKER_H
#define _GONM_WORKER_H

#include <pthread.h>
#include "gonm_context.h"

struct gonm_worker_args
{
    int server_socket;
    pthread_mutex_t* server_socket_mutex;
    struct gonm_context* context;
};

void* gonm_worker_start(void* worker_args);

#endif
