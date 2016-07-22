#ifndef _GONM_CHILD_H
#define _GONM_CHILD_H

#include <pthread.h>

struct gonm_child_args
{
    int server_socket;
    pthread_mutex_t* server_socket_mutex;
};

void* gonm_child_start(void* child_args);

#endif
