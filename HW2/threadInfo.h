#ifndef _THREAD_INFO_H
#define _THREAD_INFO_H

typedef struct ThreadInfo threadInfo;
struct ThreadInfo
{
    pthread_t inputThread;
    pthread_t senderThread;
    pthread_t receiverThread;
    pthread_t screenThread;
};

#endif