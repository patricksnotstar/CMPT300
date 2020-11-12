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
#include "sendData.h"
#include "receiver.h"
#include "threadInfo.h"
#include <signal.h>

#define MSG_MAX_LEN 1024

static pthread_t receiver_thread;
static sharedData *data;
static threadInfo *info;
static NetworkInfo *remote;
static int socketDescriptor;
static char message_in[MSG_MAX_LEN];
// sigset_t set;

void *ReceiveTx(void *args)
{
    info->receiverThread = pthread_self();
    while (!data->terminationFlag)
    {
        int bytesReceived = recvfrom(remote->local_socketDescriptor,
                                     message_in, MSG_MAX_LEN, 0,
                                     (struct sockaddr *)remote->res->ai_addr,
                                     (socklen_t *)&remote->res->ai_addrlen);

        char *message = malloc(sizeof(char) * bytesReceived);
        strcpy(message, message_in);
        pthread_mutex_lock(&data->receive_mutex);
        int incomingRequest = List_prepend(data->receiveList, message);
        data->toReadItems++;
        pthread_cond_signal(&data->readAvail);
        if (message_in[0] == '!' && message_in[1] == '\0')
        {
            char *cancelReply = &message_in[0];

            int reply = sendto(remote->local_socketDescriptor,
                               cancelReply, 1, 0, remote->res->ai_addr,
                               remote->res->ai_addrlen);
            if (reply == -1)
            {
                printf("Error sending reply from receiver");
            }
            sleep(1);
            data->terminationFlag = 1;
            data->exitCondition = '!';
        }
        pthread_mutex_unlock(&data->receive_mutex);

        if (incomingRequest != 0)
        {
            printf("\nError adding receive message to list\n");
            pthread_exit(NULL);
        }
        memset(message_in, '\0', sizeof(message_in));
    }
    pthread_exit(NULL);
    return 0;
}

void _Receiver_init(sharedData *shared_data, NetworkInfo *remoteInfo, threadInfo *thread_info)
{
    data = shared_data;
    info = thread_info;
    remote = remoteInfo;
    int receiver = pthread_create(&receiver_thread, NULL, ReceiveTx, NULL);
    if (receiver != 0)
    {
        printf("\nError creating receiver thread\n");
        return;
    }
    // data->receiverThreadID = receiver_thread;
}
void _Receiver_shutdown()
{
    pthread_join(receiver_thread, NULL);
}
