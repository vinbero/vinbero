#ifndef _GONM_COND_ARRAY_H
#define _GONM_COND_ARRAY_H

#include <libgonc/gonc_array.h>
#include <pthread.h>

struct gonm_cond_array
{
    GONC_ARRAY(pthread_cond_t*);
};

#endif
