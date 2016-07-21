#ifndef _GONM_SOCKET_LIST_H
#define _GONM_SOCKET_LIST_H

#include <libgonc/gonc_list.h>

struct gonm_socket_list_element
{
    int socket;
    GONC_LIST_ELEMENT(struct gonm_socket_list_element);
};

struct gonm_socket_list
{
    GONC_LIST(struct gonm_socket_list_element);
};

#endif
