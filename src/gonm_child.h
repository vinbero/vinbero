#ifndef _GONM_CHILD_H
#define _GONM_CHILD_H

#include <libgonc/gonc_array.h>

GONC_ARRAY(gonm_string_array, char*);

int gonm_child_receive_client_socket(int parent_socket);
void gonm_child_start(int parent_socket, struct gonm_string_array* module_path_array);

#endif
