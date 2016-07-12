#ifndef _GONM_STRING_LIST_H
#define _GONM_STRING_LIST_H

#include <libgonc/gonc_list.h>

struct gonm_string_list_element
{
    char* string;
    GONC_LIST_ELEMENT(struct gonm_string_list_element);
};

GONC_LIST(gonm_string_list, struct gonm_string_list_element);

#endif
