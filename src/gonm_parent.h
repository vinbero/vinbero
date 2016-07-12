#ifndef _GONM_PARENT_H
#define _GONM_PARENT_H

#include "gonm_socket_array.h"

struct gonm_parent_args
{
    struct gonm_socket_array* child_socket_array;
    char* address;
    int port;
    int backlog;
};

void gonm_parent_sigchld_handler(int signal_name);
ssize_t gonm_parent_send_client_socket(int child_socket, int client_socket);
void gonm_parent_start(struct gonm_parent_args* parent_args);

#endif
