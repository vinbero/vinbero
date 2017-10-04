#ifndef _TUCUBE_MODULE_H
#define _TUCUBE_MODULE_H

#include <jansson.h>
#include <pthread.h>
#include <libgenc/genc_List.h>
#include <libgenc/genc_Generic.h>

struct tucube_Module {
    void* dlHandle;
    union genc_Generic generic;
    pthread_rwlock_t* rwLock;
    pthread_key_t* tlModuleKey;
    GENC_LIST_ELEMENT(struct tucube_Module);
};

struct tucube_Module_List {
    GENC_LIST(struct tucube_Module);
};

struct tucube_Module_Config {
    json_t* json;
    GENC_LIST_ELEMENT(struct tucube_Module_Config);
};

struct tucube_Module_ConfigList {
    GENC_LIST(struct tucube_Module_Config);
};

#define TUCUBE_MODULE_DLOPEN(module, moduleConfig)                                               \
do {                                                                                             \
    if(json_string_value(json_array_get(GENC_LIST_ELEMENT_NEXT(moduleConfig)->json, 0)) == NULL) \
        errx(EXIT_FAILURE, "%s: %u: Unable to find path of next module", __FILE__, __LINE__);    \
    if(((module)->dlHandle =                                                                     \
                dlopen(json_string_value(                                                        \
                        json_array_get(GENC_LIST_ELEMENT_NEXT(moduleConfig)->json, 0)),          \
                    RTLD_LAZY | RTLD_GLOBAL)) == NULL) {                                         \
        errx(EXIT_FAILURE, "%s: %u: Failed to load next module", __FILE__, __LINE__);            \
    }                                                                                            \
}                                                                                                \
while(0)

#define TUCUBE_MODULE_DLSYM(module, modulePointerType, moduleFunction)          \
do {                                                                            \
    if((GENC_CAST((module)->generic.pointer,                                    \
         modulePointerType*)->moduleFunction =                                  \
              dlsym((module)->dlHandle, #moduleFunction)) == NULL) {            \
        errx(EXIT_FAILURE,                                                      \
             "%s: %u: Unable to find "#moduleFunction"()", __FILE__, __LINE__); \
    }                                                                           \
}                                                                               \
while(0)

#define TUCUBE_MODULE_GET_CONFIG(moduleConfig, valueName, valueType, variable, defaultValue)                         \
do {                                                                                                                 \
    if(json_object_get(json_array_get((moduleConfig)->json, 1), valueName) != NULL)                                  \
        *(variable) = json_##valueType##_value(json_object_get(json_array_get((moduleConfig)->json, 1), valueName)); \
    else                                                                                                             \
        *(variable) = defaultValue;                                                                                  \
} while(0)

#define TUCUBE_MODULE_GET_REQUIRED_CONFIG(moduleConfig, valueName, valueType, variable)                              \
do {                                                                                                                 \
    if(json_object_get(json_array_get((moduleConfig)->json, 1), valueName) != NULL)                                  \
        *(variable) = json_##valueType##_value(json_object_get(json_array_get((moduleConfig)->json, 1), valueName)); \
    else                                                                                                             \
        errx(EXIT_FAILURE, "%s: %u: Configuration argument %s is required", __FILE__, __LINE__, valueName);          \
} while(0)


#endif
