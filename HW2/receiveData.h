#ifndef _RECEIVE_DATA_H
#define _RECEIVE_DATA_H
#include <pthread.h>
#include "list.h"

typedef struct receiveData receiver;
struct receiveData
{
    List *receiveList;
    pthread_mutex_t receive_mutex;
};

#endif