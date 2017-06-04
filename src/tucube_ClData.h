#ifndef _TUCUBE_CLDATA_H
#define _TUCUBE_CLDATA_H

#include <jansson.h>
#include <libgon_c/gon_c_list.h>

struct tucube_ClData {
    union {
        int integer;
        unsigned int uInteger;
        json_t* json;
        void* pointer;
    };
    GON_C_LIST_ELEMENT(struct tucube_ClData);
};

struct tucube_ClData_List {
    GON_C_LIST(struct tucube_ClData);
};

#endif
