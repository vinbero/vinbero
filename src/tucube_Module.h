#ifndef _TUCUBE_MODULE_H
#define _TUCUBE_MODULE_H

#include <jansson.h>
#include <pthread.h>
#include <libgenc/genc_List.h>
#include <libgenc/genc_Generic.h>

struct tucube_Config {
    json_t* json;
};

struct tucube_Module {
    void* dlHandle;
    union genc_Generic generic;
    pthread_rwlock_t* rwLock;
    pthread_key_t* tlModuleKey;
    GENC_TREE_NODE(struct tucube_Module);
};

struct tucube_Module_List {
    GENC_LIST(struct tucube_Module);
};

#define TUCUBE_MODULE_DLOPEN(config, moduleName, module)                                                      \
do {                                                                                                          \
    const char* modulePath;                                                                                   \
    if((modulePath = json_string_value(json_object_get(json_object_get(config, moduleName), "path")) == NULL) \
        errx(EXIT_FAILURE, "%s: %u: Unable to find path of module %s", __FILE__, __LINE__, moduleName);       \
    if(((module)->dlHandle = dlopen(modulePath, RTLD_LAZY | RTLD_GLOBAL)) == NULL)                            \
        errx(EXIT_FAILURE, "%s: %u: Failed to load next module", __FILE__, __LINE__);                         \
} while(0)

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

#define TUCUBE_CONFIG_GET(config, moduleName, valueName, valueType, variable, defaultValue)                              \
do {                                                                                                                     \
    json_t* moduleConfig;                                                                                                \
    if((moduleConfig = json_object_get(json_object_get(json_object_get((config)->json, moduleName)), "config")) != NULL) \
        *(variable) = json_##valueType##_value(moduleConfig, valueName);                                                 \
    else                                                                                                                 \
        *(variable) = defaultValue;                                                                                      \
} while(0)

#define TUCUBE_CONFIG_GET_REQUIRED(config, moduleName, valueName, valueType, variable)                            \
do {                                                                                                                     \
    json_t* moduleConfig;                                                                                                \
    if((moduleConfig = json_object_get(json_object_get(json_object_get((config)->json, moduleName)), "config")) != NULL) \
        *(variable) = json_##valueType##_value(moduleConfig, valueName);                                                 \
    else                                                                                                                 \
        errx(EXIT_FAILURE, "%s: %u: In module %s, configuration argument %s is required", __FILE__, __LINE__, moduleName, valueName); \
} while(0)

#endif
