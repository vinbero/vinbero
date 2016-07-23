#ifndef _GONM_MASTER_H
#define _GONM_MASTER_H

#include "gonm_worker.h"

struct gonm_master_args
{
    char* address;
    int port;
    int backlog;
    size_t worker_count;
    struct gonm_worker_args* worker_args;
};

void gonm_master_start(struct gonm_master_args* master_args);

#endif
