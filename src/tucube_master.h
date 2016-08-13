#ifndef _TUCUBE_MASTER_H
#define _TUCUBE_MASTER_H

#include <unistd.h>
#include "config.h"
#include "tucube_module.h"
#include "tucube_worker.h"

struct tucube_master_args
{
    gid_t set_gid;
    uid_t set_uid;
    char* address;
    int port;
    int backlog;
    int reuse_port;
    size_t worker_count;

    struct tucube_module_args_list* module_args_list;
    struct tucube_module_list* module_list;
    void* dl_handle;
    int (*tucube_module_init)(struct tucube_module_args*, struct tucube_module_list*);
    int (*tucube_module_destroy)(struct tucube_module*);

    struct tucube_worker_args* worker_args;
};

void tucube_master_init_core(struct tucube_master_args* master_args);

void tucube_master_init_modules(struct tucube_master_args* master_args);

void tucube_master_start(struct tucube_master_args* master_args);

#endif
