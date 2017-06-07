#ifndef _TUCUBE_CORE_H
#define _TUCUBE_CORE_H

#include <stdbool.h>
#include <unistd.h>
#include <jansson.h>
#include "tucube_Module.h"
#include "tucube_IBase.h"
#include "tucube_IThreadLocal.h"

struct tucube_Core {
    const char* protocol;
    const char* address;
    int port;
    int reusePort;
    int backlog;
    size_t workerCount;
    uid_t setUid;
    gid_t setGid;
    
    bool exit;
    pthread_mutex_t* exitMutex;
    pthread_cond_t* exitCond;

    int serverSocket;
    pthread_mutex_t* serverSocketMutex;

    void* dlHandle;
    TUCUBE_IBASE_FUNCTION_POINTERS;
    TUCUBE_ITHREAD_LOCAL_FUNCTION_POINTERS;
    struct tucube_Module_List* moduleList;
};

struct tucube_Core_Config {
    json_t* json;
};

int tucube_Core_start(struct tucube_Core* core, struct tucube_Core_Config* coreConfig, struct tucube_Module_ConfigList* moduleConfigList);

#endif
