#ifndef _GONM_H
#define _GONM_H

#include "gonm_parent.h"
#include "gonm_child.h"

void gonm_start(size_t child_count, struct gonm_parent_args* parent_args, struct gonm_string_array* module_path_array);

#endif
