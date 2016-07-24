#ifndef _TCPCUBE_MASTER_H
#define _TCPCUBE_MASTER_H

#include <unistd.h>
#include "config.h"
#include "tcpcube_module.h"
#include "tcpcube_worker.h"

struct tcpcube_master_args
{
    gid_t set_gid;
    uid_t set_uid;
    char* address;
    int port;
    int backlog;
    int reuse_port;
    size_t worker_count;
    struct tcpcube_module_args_list* module_args_list;
};

void tcpcube_master_start(struct tcpcube_master_args* master_args);

#endif
