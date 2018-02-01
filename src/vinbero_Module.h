#ifndef _VINBERO_MODULE_H
#define _VINBERO_MODULE_H

#include <jansson.h>
#include <pthread.h>
#include <libgenc/genc_Generic.h>
#include <libgenc/genc_Tree.h>
#include <libgenc/genc_ArrayList.h>
#include "vinbero_IModule.h"

struct vinbero_Config;
struct vinbero_Interface;
struct vinbero_Module;

struct vinbero_Config {
    json_t* json;
};

struct vinbero_Interface {
    void* localInterface;
    struct vinbero_Module* module;
    GENC_TREE_NODE(struct vinbero_Interface, struct vinbero_Interface*);
};

struct vinbero_Module {
    const char* id;
    const char* name;
    const char* version;
    void* dlHandle;
    union genc_Generic localModule;
    //void* interface;
    struct vinbero_Interface interface;
    pthread_rwlock_t* rwLock;
    pthread_key_t* tlModuleKey;
    VINBERO_IMODULE_FUNCTION_POINTERS;
    GENC_TREE_NODE(struct vinbero_Module, struct vinbero_Module*);
};

struct vinbero_Module_Ids {
    GENC_ARRAY_LIST(const char*);
};

#define VINBERO_FUNC_ERROR -2
#define VINBERO_FUNC_EXCEPTION -1
#define VINBERO_FUNC_SUCCESS 0
#define VINBERO_FUNC_CONTINUE 1

#define VINBERO_MODULE_DLOPEN(config, module, errorVariable)                                                       \
do {                                                                                                              \
    const char* modulePath;                                                                                       \
    if((modulePath = json_string_value(json_object_get(json_object_get((config)->json, (module)->id), "path"))) == NULL) { \
        warnx("%s: %u: dlopen() failed, possible causes are:\n                                                    \
               1. Unable to find next module\n                                                                    \
               2. The next module didn't linked required shared libraries properly", __FILE__, __LINE__);         \
        *errorVariable = 1;                                                                                       \
    } else if(((module)->dlHandle = dlopen(modulePath, RTLD_LAZY | RTLD_GLOBAL)) == NULL) {                       \
        warnx("%s: %u: dlopen() failed, possible causes are:\n                                                    \
               1. Unable to find next module\n                                                                    \
               2. The next module didn't linked required shared libraries properly", __FILE__, __LINE__);         \
        *errorVariable = 1;                                                                                       \
    } else                                                                                                        \
        *errorVariable = 0;                                                                                       \
} while(0)

#define VINBERO_MODULE_DLSYM(interface, dlHandle, functionName, errorVariable) \
do {                                                                          \
    if(((interface)->functionName = dlsym(dlHandle, #functionName)) == NULL)  \
        *errorVariable = 1;                                                   \
    else                                                                      \
        *errorVariable = 0;                                                   \
}                                                                             \
while(0)

#define VINBERO_CONFIG_CHECK(config, moduleId, errorVariable)                 \
do {                                                                         \
    json_t* moduleJson;                                                      \
    json_t* moduleConfigJson;                                                \
    json_t* moduleChildrenJson;                                              \
    if((moduleJson = json_object_get((config)->json, moduleId)) != NULL      \
       && json_is_object(moduleJson)                                         \
       && (moduleConfigJson = json_object_get(moduleJson, "config")) != NULL \
       && json_is_object(moduleConfigJson)                                   \
       && (moduleChildrenJson = json_object_get(moduleJson, "next")) != NULL \
       && json_is_array(moduleChildrenJson)) {                               \
        *errorVariable = 0;                                                  \
    } else                                                                   \
        *errorVariable = 1;                                                  \
} while(0)

#define VINBERO_CONFIG_GET(config, module, valueName, valueType, output, defaultValue)                                                          \
do {                                                                                                                                           \
    bool valueFound = false;                                                                                                                   \
    json_t* outputJson;                                                                                                                        \
    for(struct vinbero_Module* currentModule = module;                                                                                          \
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

#define VINBERO_CONFIG_GET_REQUIRED(config, module, valueName, valueType, output, errorVariable)                                                \
do {                                                                                                                                           \
    *(errorVariable) = 1;                                                                                                                      \
    json_t* outputJson;                                                                                                                        \
    for(struct vinbero_Module* currentModule = module;                                                                                          \
        GENC_TREE_NODE_PARENT(currentModule) != NULL;                                                                                          \
        currentModule = GENC_TREE_NODE_PARENT(currentModule)) {                                                                                \
        if((outputJson = json_object_get(json_object_get(json_object_get((config)->json, currentModule->id), "config"), valueName)) != NULL) { \
            *(output) = json_##valueType##_value(outputJson);                                                                                  \
            *(errorVariable) = 0;                                                                                                              \
            break;                                                                                                                             \
        }                                                                                                                                      \
    }                                                                                                                                          \
} while(0)

#define VINBERO_CONFIG_GET_MODULE_PATH(config, moduleId, modulePath)                                        \
do {                                                                                                       \
    *(modulePath) = json_string_value(json_object_get(json_object_get((config)->json, moduleId), "path")); \
} while(0)

#define VINBERO_CONFIG_GET_CHILD_MODULE_COUNT(config, moduleId, output)                  \
do {                                                                                    \
    json_t* array = json_object_get(json_object_get((config)->json, moduleId), "next"); \
    if(json_is_array(array))                                                            \
        *output = json_array_size(array);                                               \
    else                                                                                \
        *output = -1;                                                                   \
} while(0)

#define VINBERO_CONFIG_GET_CHILD_MODULE_IDS(config, moduleId, output)                                 \
do {                                                                                                 \
    size_t childModuleCount;                                                                         \
    VINBERO_CONFIG_GET_CHILD_MODULE_COUNT(config, moduleId, &childModuleCount);                       \
    json_t* childModuleIdsJson = json_object_get(json_object_get((config)->json, moduleId), "next"); \
    GENC_ARRAY_LIST_REALLOC(output, childModuleCount);                                               \
    json_t* childModuleIdJson;                                                                       \
    size_t index;                                                                                    \
    json_array_foreach(childModuleIdsJson, index, childModuleIdJson)                                 \
        GENC_ARRAY_LIST_PUSH(output, json_string_value(childModuleIdJson));                          \
} while(0)

#endif
