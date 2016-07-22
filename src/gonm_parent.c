#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gonm_parent.h"

void gonm_parent_start(struct gonm_parent_args* parent_args)
{
    pthread_t* threads = malloc(parent_args->child_count * sizeof(pthread_t));
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    for(size_t index = 0; index != parent_args->child_count; ++index)
    {
        if(pthread_create(threads + index, &thread_attr, gonm_child_start, parent_args->child_args) != 0)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    while(true){sleep(1);}
}

