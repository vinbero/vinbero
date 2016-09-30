#ifndef _TUCUBE_MASTER_H
#define _TUCUBE_MASTER_H

#include <unistd.h>
#include "config.h"
#include "tucube_Module.h"
#include "tucube_Worker.h"

struct tucube_Master_Args {
    gid_t setGid;
    uid_t setUid;
    char* address;
    int port;
    int backlog;
    int reusePort;
    size_t workerCount;
    struct tucube_Module_ArgsList* moduleArgsList;
    struct tucube_Module_List* moduleList;
    void* dlHandle;
    int (*tucube_Module_init)(struct tucube_Module_Args*, struct tucube_Module_List*);
    int (*tucube_Module_destroy)(struct tucube_Module*);
    struct tucube_Worker_Args* workerArgs;
};

void tucube_Master_initCore(struct tucube_Master_Args* masterArgs);

void tucube_Master_initModules(struct tucube_Master_Args* masterArgs);

void tucube_Master_start(struct tucube_Master_Args* masterArgs);

#endif
