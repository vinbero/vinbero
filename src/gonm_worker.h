#ifndef _GONM_WORKER_H
#define _GONM_WORKER_H

#include <pthread.h>

struct gonm_worker_args
{
    int server_socket;
    pthread_mutex_t* server_socket_mutex;
};

void* gonm_worker_start(void* worker_args);

#endif
