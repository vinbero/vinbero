#ifndef _TCPCUBE_WORKER_H
#define _TCPCUBE_WORKER_H

#include <pthread.h>
#include "tcpcube_module.h"

struct tcpcube_worker_args
{
    int server_socket;
    pthread_mutex_t* server_socket_mutex;
    struct tcpcube_module* module;
    struct tcpcube_module_args* module_args;
    int (*tcpcube_module_tlinit)(struct tcpcube_module*, struct tcpcube_module_args*);
    int (*tcpcube_module_start)(struct tcpcube_module*, int*, pthread_mutex_t*);
    int (*tcpcube_module_tldestroy)(struct tcpcube_module*);
};

void* tcpcube_worker_start(void* worker_args);

#endif
