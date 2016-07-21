#include <errno.h>
#include <libgonc/gonc_array.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "gonm.h"
#include "gonm_parent.h"
#include "gonm_child.h"
#include "gonm_cond_array.h"
#include "gonm_mutex_array.h"
#include "gonm_int_array.h"

void gonm_start(struct gonm_parent_args* parent_args, struct gonm_child_args_array* child_args_array)
{
    pthread_t* threads = malloc(GONC_ARRAY_SIZE(child_args_array) * sizeof(pthread_t));
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

    for(size_t index = 0; index != GONC_ARRAY_SIZE(child_args_array); ++index)
    {
        GONC_ARRAY_GET(child_args_array, index).client_socket_cond = GONC_ARRAY_GET(parent_args->client_socket_cond_array, index);
        GONC_ARRAY_GET(child_args_array, index).client_socket_mutex = GONC_ARRAY_GET(parent_args->client_socket_mutex_array, index);
        GONC_ARRAY_GET(child_args_array, index).client_socket = &(GONC_ARRAY_GET(parent_args->client_socket_array, index));
        if(pthread_create(threads + index, &thread_attr, gonm_child_start, &(GONC_ARRAY_GET(child_args_array, index))) != 0)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    gonm_parent_start(parent_args);
}

int main(int argc, char* argv[])
{

    struct gonm_cond_array* client_socket_cond_array = malloc(sizeof(struct gonm_cond_array));
    GONC_ARRAY_INIT(client_socket_cond_array, 3);
    for(size_t index = 0; index != GONC_ARRAY_SIZE(client_socket_cond_array); ++index)
    {
        GONC_ARRAY_SET(client_socket_cond_array, index, malloc(sizeof(pthread_cond_t)));
        pthread_cond_init(GONC_ARRAY_GET(client_socket_cond_array, index), NULL);
    }

    struct gonm_mutex_array* client_socket_mutex_array = malloc(sizeof(struct gonm_mutex_array));
    GONC_ARRAY_INIT(client_socket_mutex_array, 3);
    for(size_t index = 0; index != GONC_ARRAY_SIZE(client_socket_mutex_array); ++index)
    {
        GONC_ARRAY_SET(client_socket_mutex_array, index, malloc(sizeof(pthread_mutex_t)));
        pthread_mutex_init(GONC_ARRAY_GET(client_socket_mutex_array, index), NULL);
    }

    struct gonm_int_array* client_socket_array = malloc(sizeof(struct gonm_int_array));
    GONC_ARRAY_INIT(client_socket_array, 3);
    for(size_t index = 0; index != GONC_ARRAY_SIZE(client_socket_cond_array); ++index)
        GONC_ARRAY_SET(client_socket_array, index, -1);

    struct gonm_child_args_array* child_args_array = malloc(sizeof(struct gonm_child_args_array));
    GONC_ARRAY_INIT(child_args_array, 3);

    struct gonm_parent_args* parent_args = malloc(sizeof(struct gonm_parent_args));
    parent_args->client_socket_cond_array = client_socket_cond_array;
    parent_args->client_socket_mutex_array = client_socket_mutex_array;
    parent_args->client_socket_array = client_socket_array;
    parent_args->address = "0.0.0.0";
    parent_args->port = 8080;
    parent_args->backlog = 1024;

    gonm_start(parent_args, child_args_array);

    return 0;
}
