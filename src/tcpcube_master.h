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
    struct tcpcube_module_list* module_list;
    void* dl_handle;
    int (*tcpcube_module_init)(struct tcpcube_module_args*, struct tcpcube_module_list*);
    int (*tcpcube_module_destroy)(struct tcpcube_module*);

    struct tcpcube_worker_args* worker_args;
};

void tcpcube_master_init_core(struct tcpcube_master_args* master_args);

void tcpcube_master_init_modules(struct tcpcube_master_args* master_args);

void tcpcube_master_start(struct tcpcube_master_args* master_args);

#endif
