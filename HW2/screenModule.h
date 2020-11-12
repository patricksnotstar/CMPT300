#ifndef _SCREEN_MODULE_H
#define _SCREEN_MODULE_H

#include "list.h"
#include "sendData.h"
#include "threadInfo.h"

void _Screen_init(sharedData *shared_data, threadInfo *thread_info);
void _Screen_shutdown(void);

#endif