#ifndef _TUCUBE_MASTER_H
#define _TUCUBE_MASTER_H

#include <unistd.h>
#include "config.h"
#include "tucube_Module.h"
#include "tucube_Worker.h"

struct tucube_Master {
    gid_t setGid;
    uid_t setUid;
    char* address;
    int port;
    int backlog;
    int reusePort;
    size_t workerCount;
    struct tucube_Module_ConfigList* moduleConfigList;
    struct tucube_Module_List* moduleList;
    void* dlHandle;
    int (*tucube_Module_init)(struct tucube_Module_Config*, struct tucube_Module_List*);
    int (*tucube_Module_destroy)(struct tucube_Module*);
};

void tucube_Master_initCore(struct tucube_Master* master, struct tucube_Worker* worker);

void tucube_Master_initModules(struct tucube_Master* master, struct tucube_Worker* worker);

void tucube_Master_start(struct tucube_Master* master, struct tucube_Worker* worker);

#endif
