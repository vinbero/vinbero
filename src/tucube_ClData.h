#ifndef _TUCUBE_CLDATA_H
#define _TUCUBE_CLDATA_H

#include <libgonc/gonc_list.h>

struct tucube_ClData {
    union {
        int integer;
        unsigned int uInteger;
        void* pointer;
    };
    GONC_LIST_ELEMENT(struct tucube_ClData);
};

struct tucube_ClData_List {
    GONC_LIST(struct tucube_ClData);
};

#endif
