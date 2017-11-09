#ifndef _TUCUBE_CLDATA_H
#define _TUCUBE_CLDATA_H

#include <libgenc/genc_Generic.h>
#include <libgenc/genc_Tree.h>

struct tucube_ClData {
    union genc_Generic generic;
    GENC_TREE_NODE(struct tucube_ClData);
};

#endif
