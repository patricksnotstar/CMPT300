#ifndef _RECEIVER_H
#define _RECEIVER_H
#include "network.h"
#include "list.h"
#include "sendData.h"
#include "threadInfo.h"

void _Receiver_init(sharedData *shared_data, NetworkInfo *remoteInfo, threadInfo *thread_info);
void _Receiver_shutdown(void);

#endif