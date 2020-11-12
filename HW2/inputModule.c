#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "sendData.h"
#include "list.h"
#include "inputModule.h"
#include "threadInfo.h"
#include <signal.h>

#define MSG_MAX_LEN 1024

static pthread_t input_thread;
static sharedData *data;
static threadInfo *info;
// sigset_t set;

void *getInput(void *args)
{

    // sigemptyset(&set);
    // if (sigaddset(&set, SIGUSR1) == 1)
    // {
    //     pthread_cancel(pthread_self());
    //     return 0;
    // }

    info->inputThread = pthread_self();
    while (!data->terminationFlag)
    {
        char *message = malloc(MSG_MAX_LEN);
        int newMessage;
        char firstChar;
        int messageLength;

        // fgets(message, MSG_MAX_LEN, stdin);
        read(STDIN_FILENO, message, MSG_MAX_LEN);
        // Get first character
        firstChar = message[0];
        messageLength = strlen(message);

        // Null termination
        int terminateIdx = (messageLength < MSG_MAX_LEN) ? messageLength - 1 : MSG_MAX_LEN - 1;
        message[terminateIdx] = '\0';

        // Critical section
        pthread_mutex_lock(&data->send_mutex);
        // printf("entering input CS\n");
        newMessage = List_prepend(data->sendList, message);
        data->toSendItems++;
        if (firstChar == '!' && terminateIdx == 1)
        {
            sleep(1);
            data->terminationFlag = 1;
        }
        pthread_cond_signal(&data->sendAvail);
        pthread_mutex_unlock(&data->send_mutex);
        if (newMessage != 0)
        {
            puts("\nError adding new message\n");
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
    return 0;
}

void _Input_init(sharedData *shared_data, threadInfo *thread_info)
{
    data = shared_data;
    info = thread_info;
    int input = pthread_create(&input_thread, NULL, getInput, NULL);
    if (input != 0)
    {
        printf("\nError creating input thread\n");
        return;
    }
}
void _Input_shutdown(void)
{
    pthread_join(input_thread, NULL);
}