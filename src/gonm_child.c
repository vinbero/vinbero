#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "gonm_child.h"
#include "gonm_socket_list.h"

void* gonm_child_start(void* child_args)
{
    struct gonm_socket_list_element* client_socket_list_element;
    while(true)
    {
        pthread_mutex_lock(((struct gonm_child_args*)child_args)->client_socket_mutex);
        while(GONC_LIST_SIZE(((struct gonm_child_args*)child_args)->client_socket_list) < 1)
            pthread_cond_wait(((struct gonm_child_args*)child_args)->client_socket_cond, ((struct gonm_child_args*)child_args)->client_socket_mutex);
        client_socket_list_element = GONC_LIST_HEAD(((struct gonm_child_args*)child_args)->client_socket_list);
        GONC_LIST_REMOVE(((struct gonm_child_args*)child_args)->client_socket_list, client_socket_list_element);
        pthread_mutex_unlock(((struct gonm_child_args*)child_args)->client_socket_mutex);
        

        write(client_socket_list_element->socket, "HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n", sizeof("HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n"));
        
        if(close(client_socket_list_element->socket) == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        }

        free(client_socket_list_element);
    }
    return NULL;
}
