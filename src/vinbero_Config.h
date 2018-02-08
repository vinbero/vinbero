#ifndef _VINBERO_CONFIG_H
#define _VINBERO_CONFIG_H

#include <jansson.h>
#include <libgenc/genc_Tree.h>
#include <libgenc/genc_ArrayList.h>

struct vinbero_Config {
    json_t* json;
};

#define VINBERO_CONFIG_CHECK(config, moduleId, errorVariable)                \
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

#define VINBERO_CONFIG_GET(config, module, valueName, valueType, output, defaultValue)                                                         \
do {                                                                                                                                           \
    bool valueFound = false;                                                                                                                   \
    json_t* outputJson;                                                                                                                        \
    for(struct vinbero_Module* currentModule = module;                                                                                         \
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

#define VINBERO_CONFIG_GET_REQUIRED(config, module, valueName, valueType, output, errorVariable)                                               \
do {                                                                                                                                           \
    *(errorVariable) = 1;                                                                                                                      \
    json_t* outputJson;                                                                                                                        \
    for(struct vinbero_Module* currentModule = module;                                                                                         \
        GENC_TREE_NODE_PARENT(currentModule) != NULL;                                                                                          \
        currentModule = GENC_TREE_NODE_PARENT(currentModule)) {                                                                                \
        if((outputJson = json_object_get(json_object_get(json_object_get((config)->json, currentModule->id), "config"), valueName)) != NULL) { \
            *(output) = json_##valueType##_value(outputJson);                                                                                  \
            *(errorVariable) = 0;                                                                                                              \
            break;                                                                                                                             \
        }                                                                                                                                      \
    }                                                                                                                                          \
} while(0)

#define VINBERO_CONFIG_GET_MODULE_PATH(config, moduleId, modulePath)                                       \
do {                                                                                                       \
    *(modulePath) = json_string_value(json_object_get(json_object_get((config)->json, moduleId), "path")); \
} while(0)

#define VINBERO_CONFIG_GET_CHILD_MODULE_COUNT(config, moduleId, output)                 \
do {                                                                                    \
    json_t* array = json_object_get(json_object_get((config)->json, moduleId), "next"); \
    if(json_is_array(array))                                                            \
        *output = json_array_size(array);                                               \
    else                                                                                \
        *output = -1;                                                                   \
} while(0)

#define VINBERO_CONFIG_GET_CHILD_MODULE_IDS(config, moduleId, output)                                \
do {                                                                                                 \
    size_t childModuleCount;                                                                         \
    VINBERO_CONFIG_GET_CHILD_MODULE_COUNT(config, moduleId, &childModuleCount);                      \
    json_t* childModuleIdsJson = json_object_get(json_object_get((config)->json, moduleId), "next"); \
    GENC_ARRAY_LIST_REALLOC(output, childModuleCount);                                               \
    json_t* childModuleIdJson;                                                                       \
    size_t index;                                                                                    \
    json_array_foreach(childModuleIdsJson, index, childModuleIdJson)                                 \
        GENC_ARRAY_LIST_PUSH(output, json_string_value(childModuleIdJson));                          \
} while(0)

#endif
