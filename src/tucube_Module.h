#ifndef _TUCUBE_MODULE_H
#define _TUCUBE_MODULE_H

#include <jansson.h>
#include <pthread.h>
#include <libgenc/genc_Generic.h>
#include <libgenc/genc_Tree.h>
#include <libgenc/genc_ArrayList.h>
#include "tucube_IModule.h"

struct tucube_Config {
    json_t* json;
};

struct tucube_Module {
    const char* id;
    const char* name;
    const char* version;
    void* dlHandle;
    union genc_Generic localModule;
    void* interface;
    pthread_rwlock_t* rwLock;
    pthread_key_t* tlModuleKey;
    TUCUBE_IMODULE_FUNCTION_POINTERS;
    GENC_TREE_NODE(struct tucube_Module, struct tucube_Module*);
};

struct tucube_Module_Ids {
    GENC_ARRAY_LIST(const char*);
};

#define TUCUBE_FUNC_ERROR -2
#define TUCUBE_FUNC_EXCEPTION -1
#define TUCUBE_FUNC_SUCCESS 0
#define TUCUBE_FUNC_CONTINUE 1

#define TUCUBE_MODULE_DLOPEN(config, moduleId, module, errorVariable)                                       \
do {                                                                                                        \
    const char* modulePath;                                                                                 \
    if((modulePath = json_string_value(json_object_get(json_object_get(config, moduleId), "path")) == NULL) \
        *errorVariable = 1;                                                                                 \
    if(((module)->dlHandle = dlopen(modulePath, RTLD_LAZY | RTLD_GLOBAL)) == NULL)                          \
        *errorVariable = 1;                                                                                 \
    *errorVariable = 0;                                                                                     \
} while(0)
/*
#define TUCUBE_MODULE_DLSYM(pointer, localModuleType, moduleFunction, errorVariable) \
do {                                                                                 \
    if((GENC_CAST((pointer),                                                         \
         localModuleType*)->moduleFunction =                                         \
              dlsym((module)->dlHandle, #moduleFunction)) == NULL) {                 \
        *errorVariable = 1;                                                          \
    }                                                                                \
    *errorVariable = 0;                                                              \
}                                                                                    \
while(0)
*/
#define TUCUBE_CONFIG_CHECK(config, moduleId, errorVariable)                         \
do {                                                                                 \
    json_t* moduleJson;                                                              \
    json_t* moduleConfigJson;                                                        \
    json_t* moduleChildrenJson;                                                      \
    if((moduleJson = json_object_get((config)->json, moduleId)) != NULL              \
       && json_is_object(moduleJson)                                                 \
       && (moduleConfigJson = json_object_get(moduleJson, "config")) != NULL         \
       && json_is_object(moduleConfigJson)                                           \
       && (moduleChildrenJson = json_object_get(moduleJson, "next")) != NULL         \
       && json_is_array(moduleChildrenJson)) {                                       \
        *errorVariable = 0;                                                          \
    } else                                                                           \
        *errorVariable = 1;                                                          \
} while(0)

#define TUCUBE_CONFIG_GET(config, module, valueName, valueType, output, defaultValue)                                                          \
do {                                                                                                                                           \
    bool valueFound = false;                                                                                                                   \
    json_t* outputJson;                                                                                                                        \
    for(struct tucube_Module* currentModule = module;                                                                                          \
        GENC_TREE_NODE_PARENT(currentModule) != NULL;                                                                                          \
        currentModule = GENC_TREE_NODE_PARENT(currentModule)) {                                                                                \
        if((outputJson = json_object_get(json_object_get(json_object_get((config)->json, currentModule->id), "config"), valueName)) != NULL) { \
            *(output) = json_##valueType##_value(outputJson);                                                                                  \
            valueFound = true;                                                                                                                 \
            break;                                                                                                                             \
        }                                                                                                                                      \
    }                                                                                                                                          \
    if(valueFound == false)                                                                                                                    \
        *(output) = defaultValue;                                                                                                              \
} while(0)

#define TUCUBE_CONFIG_GET_REQUIRED(config, moduleId, valueName, valueType, output, errorVariable)                                              \
do {                                                                                                                                           \
    *errorVariable = 1;                                                                                                                        \
    json_t* outputJson;                                                                                                                        \
    for(struct tucube_Module* currentModule = module;                                                                                          \
        GENC_TREE_NODE_PARENT(currentModule) != NULL;                                                                                          \
        currentModule = GENC_TREE_NODE_PARENT(currentModule)) {                                                                                \
        if((outputJson = json_object_get(json_object_get(json_object_get((config)->json, currentModule->id), "config"), valueName)) != NULL) { \
            *(output) = json_##valueType##_value(outputJson);                                                                                  \
            *errorVariable = 0;                                                                                                                \
            break;                                                                                                                             \
        }                                                                                                                                      \
    }                                                                                                                                          \
} while(0)

#define TUCUBE_CONFIG_GET_MODULE_PATH(config, moduleId, modulePath)                                        \
do {                                                                                                       \
    *(modulePath) = json_string_value(json_object_get(json_object_get((config)->json, moduleId), "path")); \
} while(0)

#define TUCUBE_CONFIG_GET_CHILD_MODULE_COUNT(config, moduleId, output)                  \
do {                                                                                    \
    json_t* array = json_object_get(json_object_get((config)->json, moduleId), "next"); \
    if(json_is_array(array))                                                            \
        *output = json_array_size(array);                                               \
    else                                                                                \
        *output = -1;                                                                   \
} while(0)

#define TUCUBE_CONFIG_GET_CHILD_MODULE_IDS(config, moduleId, output)                                 \
do {                                                                                                 \
    size_t childModuleCount;                                                                         \
    TUCUBE_CONFIG_GET_CHILD_MODULE_COUNT(config, moduleId, &childModuleCount);                       \
    json_t* childModuleIdsJson = json_object_get(json_object_get((config)->json, moduleId), "next"); \
    GENC_ARRAY_LIST_REALLOC(output, childModuleCount);                                               \
    json_t* childModuleIdJson;                                                                       \
    size_t index;                                                                                    \
    json_array_foreach(childModuleIdsJson, index, childModuleIdJson)                                 \
        GENC_ARRAY_LIST_PUSH(output, json_string_value(childModuleIdJson));                          \
} while(0)

#endif
