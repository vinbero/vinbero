#ifndef _GONM_CHILD_H
#define _GONM_CHILD_H

#include <libgonc/gonc_array.h>

struct gonm_child_args
{
    pthread_cond_t* client_socket_cond;
    pthread_mutex_t* client_socket_mutex;
    int* client_socket;
};

struct gonm_child_args_array
{
    GONC_ARRAY(struct gonm_child_args);
};

void* gonm_child_start(void* child_args);

#endif
