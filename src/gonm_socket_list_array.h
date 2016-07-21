#ifndef _GONM_SOCKET_LIST_ARRAY_H
#define _GONM_SOCKET_LIST_ARRAY_H

#include <libgonc/gonc_array.h>

struct gonm_socket_list_array
{
    GONC_ARRAY(struct gonm_socket_list*);
};

#endif
