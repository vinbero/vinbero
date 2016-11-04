#ifndef _TUCUBE_MODULE_H
#define _TUCUBE_MODULE_H

#include <jansson.h>
#include <pthread.h>
#include <libgonc/gonc_list.h>

struct tucube_Module {
    void* dlHandle;
    union {
        int integer;
        unsigned int uInteger;
        json_t* json;
        void* pointer;
    };
    pthread_rwlock_t* rwLock;
    pthread_key_t* tlModuleKey;
    GONC_LIST_ELEMENT(struct tucube_Module);
};

struct tucube_Module_List {
    GONC_LIST(struct tucube_Module);
};

struct tucube_Module_Config {
    json_t* json;
    GONC_LIST_ELEMENT(struct tucube_Module_Config);
}

struct tucube_Module_ConfigList {
    GONC_LIST(struct tucube_Module_ConfigList);
};

#define TUCUBE_MODULE_DLOPEN(module, moduleConfig)                      \
do {                                                                    \
    if(((module)->dlHandle =                                            \
                dlopen(json_string_value(                               \
                        json_array_get((moduleConfig)->json, 0)), \
                    RTLD_LAZY | RTLD_GLOBAL)) == NULL) {                \
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);                \
    }                                                                   \
}                                                                       \
while(0)

#define TUCUBE_MODULE_DLSYM(module, modulePointerType, moduleFunction)          \
do {                                                                            \
    if((GONC_CAST((module)->pointer,                                            \
         modulePointerType*)->moduleFunction =                                  \
              dlsym((module)->dlHandle, #moduleFunction)) == NULL) {            \
        errx(EXIT_FAILURE,                                                      \
             "%s: %u: Unable to find "#moduleFunction"()", __FILE__, __LINE__); \
    }                                                                           \
}                                                                               \
while(0)

#endif
