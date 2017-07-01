#ifndef _TUCUBE_CLDATA_H
#define _TUCUBE_CLDATA_H

#include <libgon_c/gon_c_list.h>
#include <libgon_c/gon_c_generic.h>

struct tucube_ClData {
    union gon_c_Generic generic;
    GON_C_LIST_ELEMENT(struct tucube_ClData);
};

struct tucube_ClData_List {
    GON_C_LIST(struct tucube_ClData);
};

#endif
