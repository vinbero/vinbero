#ifndef _GONM_H
#define _GONM_H

#include "gonm_parent.h"

void gonm_start(size_t child_count, struct gonm_socket_array* child_socket_array, const char* address, const int port, const int backlog);

#endif
