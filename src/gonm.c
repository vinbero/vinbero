#include <libgonc/gonc_array.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "gonm.h"
#include "gonm_parent.h"
#include "gonm_child.h"
#include "gonm_socket_array.h"
#include "gonm_string_list.h"

void gonm_start(size_t child_count, struct gonm_parent_args* parent_args, struct gonm_string_list* module_path_list)
{
    if(child_count == 0)
    {
        struct sigaction sigchld_action;
        sigchld_action.sa_handler = gonm_parent_sigchld_handler;
        sigemptyset(&sigchld_action.sa_mask);
        sigchld_action.sa_flags = 0;
        sigaction(SIGCHLD, &sigchld_action, NULL);
        gonm_parent_start(parent_args);
        //gonm_parent_stop();
        return;
    }
    int socket_pair[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair);
    if(fork() != 0)
    {
        close(socket_pair[1]);
        GONC_ARRAY_SET(parent_args->child_socket_array, parent_args->child_socket_array->size, socket_pair[0]);
        gonm_start(child_count - 1, parent_args, module_path_list);
    }
    else
    {
        close(socket_pair[0]);
        gonm_child_start(socket_pair[1], module_path_list);
    }
}

int main(int argc, char* argv[])
{
    struct gonm_socket_array child_socket_array;
    GONC_ARRAY_INIT(&child_socket_array, int, 3);

    struct gonm_string_list* module_path_list = malloc(sizeof(struct gonm_string_list));
    GONC_LIST_INIT(module_path_list);

    struct gonm_string_list_element* module_path = malloc(sizeof(struct gonm_string_list_element));
    GONC_LIST_ELEMENT_INIT(module_path);
    module_path->string = "/home/arch/git/gonm/src/libtest.so";
    GONC_LIST_INSERT_AFTER(module_path_list, module_path_list->tail, module_path);

    gonm_start(child_socket_array.capacity, &(struct gonm_parent_args){&child_socket_array, "0.0.0.0", 8080, 1024}, module_path_list);

    for(struct gonc_string_list_element* target_element; module_path_list->size > 0; )
    {
        target_element = module_path_list->head;
        GONC_LIST_REMOVE(module_path_list, module_path_list->head);
        free(target_element);
    }
    free(module_path_list);

    free(child_socket_array.elements);

    return 0;
}
