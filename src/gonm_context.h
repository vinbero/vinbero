#ifndef _GONM_CONTEXT_H
#define _GONM_CONTEXT_H

#include <libgonc/gonc_hmap.h>

struct gonm_context_element
{
    void* data;
    size_t data_size;
    GONC_HMAP_ELEMENT(struct gonm_context_element);
};

struct gonm_context
{
    GONC_HMAP(struct gonm_context_element);
};

#endif
