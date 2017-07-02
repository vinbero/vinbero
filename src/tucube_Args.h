#ifndef _TUCUBE_ARGS_H
#define _TUCUBE_ARGS_H
#include <stdbool.h>
static inline tucube_Args_checkCount(void* args[], int count) {
    for(int index = 0; index < count; ++index) {
        if(args[index] == NULL)
            return false;
    }
    return true;
}
#endif
