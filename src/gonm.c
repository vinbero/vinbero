#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "gonm_master.h"
#include "gonm_worker.h"

int main(int argc, char* argv[])
{
    struct gonm_master_args* master_args = malloc(sizeof(struct gonm_master_args));
    master_args->address = "0.0.0.0";
    master_args->port = 8080;
    master_args->backlog = 1024;
    master_args->worker_count = 3;
    master_args->worker_args = malloc(sizeof(struct gonm_worker_args));
    gonm_master_start(master_args);
    free(master_args->worker_args);
    free(master_args);
    return 0;
}
