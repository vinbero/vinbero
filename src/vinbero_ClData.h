#ifndef _VINBERO_CLDATA_H
#define _VINBERO_CLDATA_H

#include <libgenc/genc_Generic.h>
#include <libgenc/genc_Tree.h>

struct vinbero_ClData {
    union genc_Generic generic;
    GENC_TREE_NODE(struct vinbero_ClData, struct vinbero_ClData*);
};

#endif
