#ifndef _GONHTTPD_PARENT_H
#define _GONHTTPD_PARENT_H

#include <libgonc/gonc_list.h>

struct gonhttpd_socket_list_element
{
    int socket;
    GONC_LIST_ELEMENT(struct gonhttpd_socket_list_element);
};

GONC_LIST(gonhttpd_socket_list, struct gonhttpd_socket_list_element);

void gonhttpd_parent_sigchld_handler(int signal_name);
void gonhttpd_parent_start(struct gonhttpd_socket_list* child_socket_list);

#endif
