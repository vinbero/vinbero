#ifndef _TUCUBE_MODULE_H
#define _TUCUBE_MODULE_H

#include <jansson.h>
#include <pthread.h>
#include <libgon_c/gon_c_list.h>

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
    GON_C_LIST_ELEMENT(struct tucube_Module);
};

struct tucube_Module_List {
    GON_C_LIST(struct tucube_Module);
};

struct tucube_Module_Config {
    json_t* json;
    GON_C_LIST_ELEMENT(struct tucube_Module_Config);
};

struct tucube_Module_ConfigList {
    GON_C_LIST(struct tucube_Module_Config);
};

#define TUCUBE_MODULE_DLOPEN(module, moduleConfig)                                               \
do {                                                                                             \
    if(json_string_value(json_array_get(GON_C_LIST_ELEMENT_NEXT(moduleConfig)->json, 0)) == NULL) \
        errx(EXIT_FAILURE, "%s: %u: Unable to find path of next module", __FILE__, __LINE__);    \
    if(((module)->dlHandle =                                                                     \
                dlopen(json_string_value(                                                        \
                        json_array_get(GON_C_LIST_ELEMENT_NEXT(moduleConfig)->json, 0)),          \
                    RTLD_LAZY | RTLD_GLOBAL)) == NULL) {                                         \
        errx(EXIT_FAILURE, "%s: %u: Failed to load next module", __FILE__, __LINE__);            \
    }                                                                                            \
}                                                                                                \
while(0)

#define TUCUBE_MODULE_DLSYM(module, modulePointerType, moduleFunction)          \
do {                                                                            \
    if((GON_C_CAST((module)->pointer,                                           \
         modulePointerType*)->moduleFunction =                                  \
              dlsym((module)->dlHandle, #moduleFunction)) == NULL) {            \
        errx(EXIT_FAILURE,                                                      \
             "%s: %u: Unable to find "#moduleFunction"()", __FILE__, __LINE__); \
    }                                                                           \
}                                                                               \
while(0)

#endif
