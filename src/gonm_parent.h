#ifndef _GONM_PARENT_H
#define _GONM_PARENT_H

#include <libgonc/gonc_array.h>

GONC_ARRAY(gonm_socket_array, int);

void gonm_parent_sigchld_handler(int signal_name);
void gonm_parent_send_client_socket(int child_socket, int client_socket);
void gonm_parent_start(struct gonm_socket_array* child_socket_array);

#endif
