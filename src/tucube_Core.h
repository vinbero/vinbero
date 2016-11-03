#ifndef _TUCUBE_CORE_H
#define _TUCUBE_CORE_H

#include <stdbool.h>
#include <jansson.h>

struct tucube_Core {
    struct tucube_Module_List* moduleList;
    void* dlHandle;
    int (*tucube_Module_init)(struct tucube_Module_Config*, struct tucube_Module_List*);
    int (*tucube_Module_destroy)(struct tucube_Module*);
    int (*tucube_Module_tlInit)(struct tucube_Module*, struct tucube_Module_Config*);
    int (*tucube_Module_start)(struct tucube_Module*, int*, pthread_mutex_t*);
    int (*tucube_Module_tlDestroy)(struct tucube_Module*);
    bool exit;
    pthread_mutex_t* exitMutex;
    pthread_cond_t* exitCond;
    int serverSocket;
    pthread_mutex_t* serverSocketMutex;
};

struct tucube_Core_Config {
    json_t* jsonObject;
};

void* tucube_Core_startWorker(void* core);

#endif
