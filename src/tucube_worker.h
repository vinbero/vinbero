#ifndef _TUCUBE_WORKER_H
#define _TUCUBE_WORKER_H

#include <pthread.h>
#include <stdbool.h>
#include "config.h"
#include "tucube_module.h"

struct tucube_worker_args
{
    bool exit;
    pthread_mutex_t* exit_mutex;
    pthread_cond_t* exit_cond;
    int server_socket;
    pthread_mutex_t* server_socket_mutex;
    struct tucube_module* module;
    struct tucube_module_args* module_args;
    int (*tucube_module_tlinit)(struct tucube_module*, struct tucube_module_args*);
    int (*tucube_module_start)(struct tucube_module*, int*, pthread_mutex_t*);
    int (*tucube_module_tldestroy)(struct tucube_module*);
};

void* tucube_worker_start(void* worker_args);

#endif
