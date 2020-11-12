#ifndef _INPUT_MODULE_H
#define _INPUT_MODULE_H
#include "list.h"
#include "sendData.h"
#include "threadInfo.h"

void _Input_init(sharedData *shared_data, threadInfo *thread_info);
void _Input_shutdown(void);

#endif