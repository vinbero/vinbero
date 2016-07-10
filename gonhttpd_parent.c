#include <libgonc/gonc_list.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include "gonhttpd_parent.h"

void gonhttpd_parent_sigchld_handler(int signal_name)
{
    if(signal_name == SIGCHLD)
    {
        printf("SIGCHLD!\n");
        int status;
        pid_t child_pid = waitpid(-1, &status, WNOHANG);
        if(WIFEXITED(status))
        {
            printf("child %u eixted\n", child_pid);
            printf("child exit status: %d\n", WEXITSTATUS(status));
        }
        else
        {
            printf("Child abnormally exited");
        }
    }
}

void gonhttpd_parent_start(struct gonhttpd_socket_list* child_socket_list)
{
    while(true)
    {
        sleep(1);
        GONC_LIST_FOR_EACH(child_socket_list, struct gonhttpd_socket_list_element, element)
        {
            printf("PARENT pid: %u, child socket: %d \n", getpid(), element->socket);
        }
    }
}

