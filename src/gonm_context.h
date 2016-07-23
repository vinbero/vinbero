#ifndef _GONM_CONTEXT_H
#define _GONM_CONTEXT_H

#include <libgonc/gonc_hmap.h>

struct gonm_context_attribute
{
    void* data;
    size_t data_size;
    int (*destroy)(struct gonm_context_attribute*);
    GONC_HMAP_ELEMENT(struct gonm_context_attribute);
};

struct gonm_context
{
    GONC_HMAP(struct gonm_context_attribute);
};

#endif
