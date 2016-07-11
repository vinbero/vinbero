#ifndef _GONHTTPD_CHILD_H
#define _GONHTTPD_CHILD_H

int gonhttpd_child_receive_client_socket(int parent_socket);
void gonhttpd_child_start(int parent_socket);

#endif
