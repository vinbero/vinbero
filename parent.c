#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include "child.h"

void sigchld_handler(int signal_name)
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

void spawn_children(size_t n)
{
    int socket_pair[2];
    if(n == 0)
    {
        struct sigaction sigchld_action;
        sigchld_action.sa_handler = sigchld_handler;
        sigemptyset(&sigchld_action.sa_mask);
        sigchld_action.sa_flags = 0;
        sigaction(SIGCHLD, &sigchld_action, NULL);
        return;
    }
    socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair);
    if(fork() != 0)
    {
//       socket_list.add(socket_pair[0]);
       close(socket_pair[1]);
       spawn_children(n - 1);
    }
    else
    {
        close(socket_pair[0]);
        do_child_work(socket_pair[1]);
    }
}
