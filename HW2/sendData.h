#ifndef _SEND_DATA_H
#define _SEND_DATA_H
#include <pthread.h>
#include "list.h"

typedef struct sendData sharedData;
struct sendData
{
    List *sendList;
    List *receiveList;
    pthread_mutex_t send_mutex;
    pthread_mutex_t receive_mutex;
    pthread_cond_t sendAvail;
    pthread_cond_t readAvail;
    int terminationFlag;
    int toSendItems;
    int toReadItems;
};

#endif