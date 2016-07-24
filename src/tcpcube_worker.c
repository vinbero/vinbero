#include <err.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include "config.h"
#include "tcpcube_worker.h"

void* tcpcube_worker_start(void* worker_args)
{
    for(int client_socket;;)
    {
        pthread_mutex_lock(((struct tcpcube_worker_args*)worker_args)->server_socket_mutex);
        if((client_socket = accept(((struct tcpcube_worker_args*)worker_args)->server_socket, NULL, NULL)) == -1)
        {
            pthread_mutex_unlock(((struct tcpcube_worker_args*)worker_args)->server_socket_mutex);
            warn("%s: %u", __FILE__, __LINE__);
            continue;
        }
        pthread_mutex_unlock(((struct tcpcube_worker_args*)worker_args)->server_socket_mutex);

        if(((struct tcpcube_worker_args*)worker_args)->tcpcube_module_service(((struct tcpcube_worker_args*)worker_args)->module, &client_socket) == -1)
            warn("%s: %u", __FILE__, __LINE__);
        
        if(close(client_socket) == -1)
            warn("%s: %u", __FILE__, __LINE__);
    }
    return NULL;
}
