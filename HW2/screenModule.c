#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "sendData.h"
#include "list.h"
#include "screenModule.h"
#include "threadInfo.h"
#include <signal.h>

static sharedData *data;
static threadInfo *info;
pthread_t screen_thread;
// sigset_t set;

void *postScreen(void *args)
{
    char *message;
    info->screenThread = pthread_self();
    while (!data->terminationFlag)
    {
        if (data->toReadItems >= 1)
        {
            pthread_mutex_lock(&data->receive_mutex);
            if (List_count(data->receiveList) == 0)
            {
                pthread_cond_wait(&data->readAvail, &data->receive_mutex);
            }
            message = List_trim(data->receiveList);
            data->toReadItems--;
            if (message[0] == '!' && strlen(message) == 1)
            {
                sleep(1);
                data->terminationFlag = 1;
                pthread_cancel(info->inputThread);
            }
            pthread_mutex_unlock(&data->receive_mutex);

            // Display message on screen
            printf("\n%s\n", message);

            // Free memory
            free(message);
        }
    }
    pthread_exit(NULL);
    return 0;
}

void _Screen_init(sharedData *shared_data, threadInfo *thread_info)
{
    data = shared_data;
    info = thread_info;
    int input = pthread_create(&screen_thread, NULL, postScreen, NULL);
    if (input != 0)
    {
        printf("\nError creating screen thread\n");
        return;
    }
    // data->screenThreadID = screen_thread;
}
void _Screen_shutdown(void)
{
    pthread_join(screen_thread, NULL);
}