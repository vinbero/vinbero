#ifndef _TCPCUBE_WORKER_H
#define _TCPCUBE_WORKER_H

#include <pthread.h>

struct tcpcube_worker_args
{
    int server_socket;
    pthread_mutex_t* server_socket_mutex;
    struct tcpcube_module* module;
    int (*tcpcube_module_service)(struct tcpcube_module*, int*);
};

void* tcpcube_worker_start(void* worker_args);

#endif
