#ifndef _GONM_H
#define _GONM_H

#include "gonm_parent.h"
#include "gonm_child.h"
#include "gonm_string_list.h"

void gonm_start(size_t child_count, struct gonm_parent_args* parent_args, struct gonm_string_list* module_path_list);

#endif
