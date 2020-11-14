#include "OSSim.h"

int pid = 0;
List *readyQ[3];
List *sendQ;
List *recieveQ;

PCB *runningProcess;
PCB *init;

bool compareInts(void *pItem, void *pComparisonArg)
{
    return ((PCB *)pItem)->pid == *(int *)pComparisonArg ? true : false;
}

void freeItem(void *pItem)
{
    free(pItem);
}

void killProcess(PCB *processToKill)
{
    List_free(processToKill->msg, freeItem);
    free(processToKill);
}

void nextProcess()
{
    int prio = 0;
    while (prio < 3)
    {
        if (List_count(readyQ[prio]) == 0)
        {
            prio++;
        }
        else
        {
            break;
        }
    }

    if (prio == 3)
    {
        init->state = RUNNING;
        runningProcess = init;
        printf("Running init\n");
        return;
    }
    else
    {
        PCB *nextProcess = (PCB *)List_trim(readyQ[prio]);
        nextProcess->state = RUNNING;
        runningProcess = nextProcess;
        if (List_count(runningProcess->msg) > 0)
        {
            printf("Running process %d, message is: %s\n", runningProcess->pid, List_trim(runningProcess->msg));
        }
    }
}

void create(int prio)
{
    PCB *newProcess;
    newProcess = malloc(sizeof(PCB));

    if (newProcess == NULL)
    {
        printf("Failed to create a new process: Not enough memory for a new process \n");
        return;
    }

    newProcess->pid = pid++;
    newProcess->priority = prio;
    newProcess->msg = List_create();

    if (newProcess->msg == NULL)
    {
        printf("Failed to create a new process: No more space for a new process, please terminate a process then try again. \n");
        return;
    }

    if (runningProcess == init)
    {
        init->state = READY;
        newProcess->state = RUNNING;
        runningProcess = newProcess;
    }
    else
    {
        newProcess->state = READY;
        List_prepend(readyQ[prio], newProcess);
    }

    printf("Successfully created a new process with PID: %d\n", newProcess->pid);
}

void fork()
{

    if (runningProcess == init)
    {
        printf("Cannot fork init ya fool!");
        return;
    }

    PCB *newProcess;
    newProcess = malloc(sizeof(PCB));

    if (newProcess == NULL)
    {
        printf("Failed to fork a process: Not enough memory for a new process \n");
        return;
    }

    newProcess->pid = pid++;
    newProcess->priority = runningProcess->priority;
    newProcess->msg = List_create();

    if (newProcess->msg == NULL)
    {
        printf("Failed to fork a process: No more space for a new process, please terminate a process then try again \n");
        return;
    }

    newProcess->state = READY;
    List_prepend(readyQ[runningProcess->priority], newProcess);

    printf("Successfully forked a new process with PID: %d\n", newProcess->pid);
}

void kill(int pid)
{
    if (runningProcess->pid == pid)
    {
        killProcess(runningProcess);
        nextProcess();
        return;
    }
    void *processToKill;
    int prio = 0;
    while (prio < 3)
    {
        if (List_count(readyQ[prio]) == 0)
        {
            prio++;
        }
        else
        {
            List_first(readyQ[prio]);
            processToKill = List_search(readyQ[prio], compareInts, &pid);
            if (processToKill == NULL)
            {
                prio++;
            }
            else
            {
                killProcess(processToKill);
                List_remove(readyQ[prio]);
                printf("Sucessfully killed process %d\n", pid);
                return;
            }
        }
    }

    if (List_count(sendQ) > 0)
    {
        List_first(sendQ);
        processToKill = List_search(sendQ, compareInts, &pid);
        if (processToKill != NULL)
        {
            killProcess(processToKill);
            List_remove(sendQ);
            printf("Sucessfully killed process %d\n", pid);
            return;
        }
    }
    else if (List_count(recieveQ) > 0)
    {
        List_first(recieveQ);
        processToKill = List_search(recieveQ, compareInts, &pid);
        if (processToKill != NULL)
        {
            killProcess(processToKill);
            List_remove(recieveQ);
            printf("Sucessfully killed process %d\n", pid);
            return;
        }
    }
    printf("Cannot find process with pid %d\n", pid);
}

void exit_() {}
void quantum() {}
void send(int pid, char *msg) {}
void receive() {}
void reply(int pid, char *msg) {}
void newSemaphore(int sid, int initValue) {}
void P(int sid) {}
void V(int sid) {}
void procInfo(int pid) {}
void totalInfo() {}

void displayMenu()
{
    printf("Type C to create a new process \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type F to fork a process \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type K to kill a process \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type Q to kick a process out \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type S to send a message to a process \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type R to recieve a message \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type Y to reply to a process \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type N to initialize a new semaphore \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type P to execute semaphore P operation \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type V to execute semaphore V operation \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type I to see all processes' state \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Type T to display all process queues and their contents \n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("Enter your command here: ");
}

int main()
{
    readyQ[0] = List_create();
    readyQ[1] = List_create();
    readyQ[2] = List_create();
    sendQ = List_create();
    recieveQ = List_create();

    init = malloc(sizeof(PCB));

    init->pid = pid++;
    init->priority = 3;
    init->msg = NULL;
    init->state = RUNNING;
    runningProcess = init;

    char command;
    bool exit = false;

    int param1;
    int param2;
    char msg[MAX_MSG_LEN];
    displayMenu();
    while (!exit)
    {
        scanf("%c", &command);

        if (command == '\n')
        {
            printf("Enter your command here: ");
            continue;
        }
        printf("\n");
        switch (tolower(command))
        {
        case 'c':
            printf("Please assign a priority for the new process: ");
            scanf("%d", &param1);
            printf("\n");
            if (param1 != 0 && param1 != 1 && param1 != 2)
            {
                printf("Please try again and this time enter a number from 0 to 2.\n");
                continue;
            }
            create(param1);
            break;
        case 'f':
            fork();
            break;
        case 'k':
            printf("Please enter a pid for the process you want to kill: ");
            scanf("%d", &param1);
            printf("\n");
            kill(param1);
            break;
        case 'e':
            exit_();
            break;
        case 'q':
            quantum();
            break;
        case 's':
            printf("Please enter a pid for the receiving process: ");
            scanf("%d", &param1);
            printf("\n");
            printf("Please enter a message to send: ");
            fgets(msg, (MAX_MSG_LEN - 1), stdin);
            send(param1, msg);
            break;
        case 'r':
            receive();
            break;
        case 'y':
            printf("Please enter a pid for the receiving process: ");
            scanf("%d", &param1);
            printf("\n");
            printf("Please enter a message to reply: ");
            fgets(msg, (MAX_MSG_LEN - 1), stdin);
            reply(param1, msg);
            break;
        case 'n':
            printf("Please enter a semaphore ID: ");
            scanf("%d", &param1);
            printf("\n");
            printf("Please enter an initial value for the semaphore: ");
            scanf("%d", &param2);
            printf("\n");
            newSemaphore(param1, param2);
            break;
        case 'p':
            printf("Please enter a semaphore id: ");
            scanf("%d", &param1);
            printf("\n");
            P(param1);
            break;
        case 'v':
            printf("Please enter a semaphore id: ");
            scanf("%d", &param1);
            printf("\n");
            V(param1);
            break;
        case 'i':
            printf("Please enter a pid for the receiving process: ");
            scanf("%d", &param1);
            printf("\n");
            procInfo(param1);
            break;
        case 't':
            totalInfo();
            break;
        case 'm':
            displayMenu();
            break;
        }
    }

    return 0;
}