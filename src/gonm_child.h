#ifndef _GONM_CHILD_H
#define _GONM_CHILD_H

int gonm_child_receive_client_socket(int parent_socket);
void gonm_child_start(int parent_socket);

#endif
