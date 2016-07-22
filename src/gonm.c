#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "gonm_parent.h"
#include "gonm_child.h"

int main(int argc, char* argv[])
{
    struct gonm_parent_args* parent_args = malloc(sizeof(struct gonm_parent_args));
    parent_args->address = "0.0.0.0";
    parent_args->port = 8080;
    parent_args->backlog = 1024;
    parent_args->child_count = 3;
    parent_args->child_args = malloc(sizeof(struct gonm_child_args));
    gonm_parent_start(parent_args);
    return 0;
}
