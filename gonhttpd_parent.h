#ifndef _GONHTTPD_PARENT_H
#define _GONHTTPD_PARENT_H

#include <libgonc/gonc_array.h>

GONC_ARRAY(gonhttpd_socket_array, int);

void gonhttpd_parent_sigchld_handler(int signal_name);
void gonhttpd_parent_start(struct gonhttpd_socket_array* child_socket_array);

#endif
