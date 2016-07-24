#ifndef _GONM_MASTER_H
#define _GONM_MASTER_H

#include <unistd.h>
#include "gonm_module.h"
#include "gonm_worker.h"

struct gonm_master_args
{
    uid_t set_uid;
    char* address;
    int port;
    int backlog;
    int reuse_port;
    size_t worker_count;
    struct gonm_module_args_list* module_args_list;
};

void gonm_master_start(struct gonm_master_args* master_args);

#endif
