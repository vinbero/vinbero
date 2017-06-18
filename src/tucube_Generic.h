#ifndef _TUCUBE_GENERIC_H
#define _TUCUBE_GENERIC_H

#include <jansson.h>

#define TUCUBE_GENERIC     \
union {                    \
    int integer;           \
    unsigned int uIntiger; \
    void* pointer;         \
    void** array;          \
    json_t* json;          \
}

#endif
