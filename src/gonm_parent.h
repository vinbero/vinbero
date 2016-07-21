#ifndef _GONM_PARENT_H
#define _GONM_PARENT_H

#include "gonm_cond_array.h"
#include "gonm_mutex_array.h"
#include "gonm_int_array.h"

struct gonm_parent_args
{
    struct gonm_cond_array* client_socket_cond_array;
    struct gonm_mutex_array* client_socket_mutex_array;
    struct gonm_int_array* client_socket_array;
    char* address;
    int port;
    int backlog;
};

void gonm_parent_start(struct gonm_parent_args* parent_args);

#endif
