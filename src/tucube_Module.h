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

struct tucube_Module_ConfigTreeNode {
    size_t childCount;
    TUCUBE_MODULE_TREE_NODE(struct tucube_Module_ConfigTreeNode);
};

#define TUCUBE_MODULE_TREE_NODE(type) \
size_t childCount;                    \
GENC_LIST(type)

#define TUCUBE_MODULE_TREE_NODE_CHILD_COUNT(node) \
(node)->childCount

#define TUCUBE_MODULE_TREE_GET_LAST_CHILD(_node, child) \
do {
    if(TUCUBE_MODULE_TREE_NODE_CHILD_COUNT(_node) == 0)
        (*child) = NULL;
    size_t backwardCount = 0;
    type* node = _node;
    while(GENC_LIST_ELEMENT_PREVIOUS(node) != NULL &&
           GENC_MODULE_TREE_NODE_CHILD_COUNT(GENC_LIST_ELEMENT_PREVIOUS(node) != 0) {
        node = node.prev;
        ++backwardCount;
    }
    type* lastChild = GENC_LIST_ELEMENT_FORWARD()
} while(0)
#define TUCUBE_MODULE_TREE_GET_FIRST_CHILD(node, child) \


#define TUCUBE_MODULE_TREE_APPEND_CHILD(node, child) \
do {
    type* lastChild = TUCUBE_MODULE_TREE_GET_LAST_CHILD(node);
    type* prevLastChild = NULL;
    if(GENC_LIST_ELEMENT_PREVIOUS(node) != NULL)
        prevLastChild = TUCUBE_MODULE_TREE_GET_LAST_CHILD(GENC_LIST_ELEMENT_PREVIOUS(node));
    if(lastChild == NULL) {
        if(prevLastChild == NULL)
            GENC_LIST_ELEMENT_APPEND(node, child);
        else
            GENC_LIST_ELEMENT_APPEND(prevLastChild, child);
    } else
        GENC_LIST_ELEMENT_APPEND(lastChild, child);
    ++TUCUBE_MODULE_TREE_NODE_CHILD_COUNT(node);
} while(0)

#define TUCUBE_MODULE_TREE_FOR_EACH_NODE(_node, type) \
for(type* node = _node; GENC_LIST_ELEMENT_NEXT(node) != NULL; node = GENC_LIST_ELEMENT_NEXT(node))

#define TUCUBE_MODULE_TREE_FOR_EACH_CHILD_NODE_BEGIN(_node, type) \
do {                                                              \
    type* begin;                                                  \
    type* end;                                                    \
    type* node;                                                   \
    TUCUBE_MODULE_TREE_GET_FIRST_CHILD(_node, begin);             \
    TUCUBE_MODULE_TREE_GET_LAST_CHILD(_node, end);                \
    if(begin == NULL || end == NULL)                              \
        break;                                                    \
    node = begin;                                                 \
    end = GENC_LIST_ELEMENT_NEXT(end)                             \
    while(node != end) {                                          \
        node = GENC_LIST_ELEMENT_NEXT(current);

#define TUCUBE_MODULE_TREE_FOR_EACH_CHILD_NODE_END()              \
    }                                                             \
} while(0)

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
