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

void* gonm_child_start(void* child_args)
{
/*
    int (*gonm_module_run)(int, struct gonm_string_list*);
    void* dl_handle;

    if((dl_handle = dlopen(module_path_list->head->string, RTLD_LAZY)) == NULL)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    if((gonm_module_run = dlsym(dl_handle, "gonm_module_run")) == NULL)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, dlerror());
        exit(EXIT_FAILURE);
    }
*/
    int client_socket;
    while(true)
    {
        pthread_mutex_lock(((struct gonm_child_args*)child_args)->client_socket_mutex);
        while(*(((struct gonm_child_args*)child_args)->client_socket) == -1)
        {
            pthread_cond_wait(((struct gonm_child_args*)child_args)->client_socket_cond, ((struct gonm_child_args*)child_args)->client_socket_mutex);

            client_socket = *(((struct gonm_child_args*)child_args)->client_socket);
        
            *(((struct gonm_child_args*)child_args)->client_socket) = -1;
        
//           gonm_module_run(client_socket, module_path_list);

            write(client_socket, "HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n", sizeof("HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n"));
        
            if(close(client_socket) == -1)
            {
                fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
            }
        }

        pthread_mutex_unlock(((struct gonm_child_args*)child_args)->client_socket_mutex);

    }
//    dlclose(dl_handle);
    return NULL;
}
