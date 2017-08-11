#ifndef _TUCUBE_CLDATA_H
#define _TUCUBE_CLDATA_H

#include <libgenc/genc_List.h>
#include <libgenc/genc_generic.h>

struct tucube_ClData {
    union genc_Generic generic;
    GENC_LIST_ELEMENT(struct tucube_ClData);
};

struct tucube_ClData_List {
    GENC_LIST(struct tucube_ClData);
};

#endif
