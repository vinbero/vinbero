#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

void gonhttpd_child_start(int parent_socket)
{
    while(true)
    {
        sleep(1);
        printf("CHILD pid: %u, parent_socket: %d \n", getpid(), parent_socket);
    }
}
