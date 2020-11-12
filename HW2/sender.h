#ifndef _SENDER_H
#define _SENDER_H
#include "network.h"
#include "list.h"
#include "sendData.h"
#include "threadInfo.h"

void _Sender_init(sharedData *shared_data, NetworkInfo *remoteInfo, threadInfo *thread_info);
void _Sender_shutdown(void);

#endif