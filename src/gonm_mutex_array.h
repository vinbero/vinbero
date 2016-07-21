#ifndef _GONM_MUTEX_ARRAY_H
#define _GONM_MUTEX_ARRAY_H

#include <libgonc/gonc_array.h>
#include <pthread.h>

struct gonm_mutex_array
{
    GONC_ARRAY(pthread_mutex_t*);
};

#endif
