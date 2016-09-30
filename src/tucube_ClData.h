#ifndef _TUCUBE_CLDATA_H
#define _TUCUBE_CLDATA_H

#include <libgonc/gonc_list.h>

struct tucube_cldata
{
    union
    {
        int integer;
        unsigned int uinteger;
        void* pointer;
    };
    GONC_LIST_ELEMENT(struct tucube_cldata);
};

struct tucube_cldata_list
{
    GONC_LIST(struct tucube_cldata);
};

#endif
