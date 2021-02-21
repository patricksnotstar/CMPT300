#include <pthread.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "network.h"
#include "list.h"
#include "sender.h"
#include "sendData.h"
#include "threadInfo.h"
#include <signal.h>

static pthread_t sender_thread;
static sharedData *data;
static threadInfo *info;
static NetworkInfo *remote;
// sigset_t set;

void *SenderTx(void *args)
{
    char *message;
    info->senderThread = pthread_self();
    while (!data->terminationFlag)
    {
        if (data->toSendItems >= 1)
        {
            pthread_mutex_lock(&data->send_mutex);
            if (List_count(data->sendList) == 0)
            {
                pthread_cond_wait(&data->sendAvail, &data->send_mutex);
            }
            message = List_trim(data->sendList);
            data->toSendItems--;
            if (message[0] == '!' && strlen(message) == 1)
            {
                sleep(1);
                data->terminationFlag = 1;
            }
            pthread_mutex_unlock(&data->send_mutex);

            // TODO: Send data to other peer
            int reply = sendto(remote->local_socketDescriptor, message, strlen(message), 0, remote->res->ai_addr, remote->res->ai_addrlen);
            if (reply == -1)
            {
                printf("\nError sending message\n");
                return NULL;
            }
            // Deallocate message after sending
            free(message);
        }
    }
    pthread_exit(NULL);
    return 0;
}

void _Sender_init(sharedData *shared_data, NetworkInfo *remoteInfo, threadInfo *thread_info)
{
    data = shared_data;
    info = thread_info;
    remote = remoteInfo;
    int sender = pthread_create(&sender_thread, NULL, SenderTx, NULL);
    if (sender != 0)
    {
        printf("\nError creating sender thread\n");
        return;
    }
    // data->senderThreadID = sender_thread;
}
void _Sender_shutdown(void)
{
    pthread_join(sender_thread, NULL);
}
