#ifndef _VINBERO_INTERFACE_H
#define _VINBERO_INTERFACE_H

#include <libgenc/genc_Tree.h>
#include "vinbero_Module.h"

struct vinbero_Interface {
    void* localInterface;
    struct vinbero_Module* module;
    GENC_TREE_NODE(struct vinbero_Interface, struct vinbero_Interface*);
};

#endif
