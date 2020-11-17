#include "OSSim.h"

int pid = 0;
static int numProc = 0;
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

void killProcess(PCB *processInQuestion)
{
    List_free(processInQuestion->msg, freeItem);
    free(processInQuestion);
}

void printMessages(List *messages, Code code)
{
    int numMsg = List_count(messages);
    if (numMsg == 0)
    {
        printf("* This process has 0 messages\n");
    }
    else
    {
        List_first(messages);
        printf("* Messages:\n");
        if (code == NEXT)
        {
            printf("        ** %s\n", (char *)List_trim(messages));
        }
        else
        {
            for (int i = 0; i < numMsg; i++)
            {
                printf("    ** %s\n", (char *)messages->current->item);
                List_next(messages);
            }
        }
    }
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
        printf("Running process %d: \n", runningProcess->pid);
        printMessages(runningProcess->msg, NEXT);
    }
}

void printProcInfo(PCB *process)
{
    const char *states[] = {"READY", "BLOCK", "RUNNING"};
    printf("Process with pid %d has: \n", process->pid);
    printf("    * Current state is: %s\n", states[process->state]);
    printf("    * Priority is: %d\n", process->priority);
    printf("    ");
    printMessages(process->msg, INFO);
}

PCB *searchAndDoStuff(int pid, Code code)
{
    PCB *processInQuestion;
    if (runningProcess->pid == pid)
    {
        if (code == KILL)
        {
            killProcess(runningProcess);
            printf("Sucessfully killed process %d\n", pid);
            nextProcess();
            numProc--;
            return NULL;
        }
        else
        {
            printProcInfo(runningProcess);
        }
        return NULL;
    }
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
            processInQuestion = List_search(readyQ[prio], compareInts, &pid);
            if (processInQuestion == NULL)
            {
                prio++;
            }
            else
            {
                if (code == KILL)
                {
                    killProcess(processInQuestion);
                    List_remove(readyQ[prio]);
                    printf("Sucessfully killed process %d\n", pid);
                    numProc--;
                    return NULL;
                }
                else if (code == INFO)
                {
                    printProcInfo(processInQuestion);
                }

                return processInQuestion;
            }
        }
    }

    if (List_count(sendQ) > 0)
    {
        List_first(sendQ);
        processInQuestion = List_search(sendQ, compareInts, &pid);
        if (processInQuestion != NULL)
        {
            if (code == KILL)
            {
                killProcess(processInQuestion);
                List_remove(sendQ);
                printf("Sucessfully killed process %d\n", pid);
                numProc--;
                return NULL;
            }
            else if (code == INFO)
            {
                printProcInfo(processInQuestion);
            }
            return processInQuestion;
        }
    }
    else if (List_count(recieveQ) > 0)
    {
        List_first(recieveQ);
        processInQuestion = List_search(recieveQ, compareInts, &pid);
        if (processInQuestion != NULL)
        {
            if (code == KILL)
            {
                killProcess(processInQuestion);
                List_remove(recieveQ);
                printf("Sucessfully killed process %d\n", pid);
                numProc--;
                return NULL;
            }
            else if (code == INFO)
            {
                printProcInfo(processInQuestion);
            }
            return processInQuestion;
        }
    }
    printf("Cannot find process with pid %d\n", pid);
    return false;
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
    numProc++;
}

void fork()
{

    if (runningProcess == init)
    {
        printf("Cannot fork init ya fool!\n");
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
    List_first(runningProcess->msg);
    for (int i = 0; i < List_count(runningProcess->msg); i++)
    {
        size_t msgLen = strlen((char *)runningProcess->msg->current->item);
        char *dupMsg = malloc((msgLen) * sizeof(char));
        strncpy(dupMsg, (char *)runningProcess->msg->current->item, msgLen);
        List_prepend(newProcess->msg, dupMsg);
    }

    newProcess->state = READY;
    List_prepend(readyQ[runningProcess->priority], newProcess);

    printf("Successfully forked a new process with PID: %d\n", newProcess->pid);
    numProc++;
}

bool kill(int pid)
{

    if (pid == init->pid)
    {
        if (numProc != 0)
        {
            printf("Cannot kill init process because there are still other processes in the system\n");
            return false;
        }
        else
        {
            killProcess(init);
            printf("Sucessfully killed init\n");
            printf("Shutting down...bye\n");
            return true;
        }
    }

    searchAndDoStuff(pid, KILL);
    return false;
}

bool exit_()
{
    if (runningProcess->pid != init->pid)
    {
        killProcess(runningProcess);
        nextProcess();
        return false;
    }
    else
    {
        if (List_count(sendQ) == 0 && List_count(recieveQ) == 0)
        {
            printf("Shutting down... Bye\n");
            return true;
        }
        else
        {
            printf("Cannot terminate init process because there is another process alive\n");
            return false;
        }
    }
}

void quantum()
{
    if (runningProcess->pid != init->pid)
    {
        PCB *currentProcess = runningProcess;
        nextProcess();
        printf("Process %d expired\n", currentProcess->pid);
        List_prepend(readyQ[currentProcess->priority], currentProcess);
        currentProcess->state = READY;
    }
    else
    {
        printf("Current running process is init, which does not have a time quantum, please use exit instead\n");
    }
}

void send(int pid, char *msg)
{
    if (runningProcess->pid == pid)
    {
        printf("You cannot send a message to the current running process\n");
        return;
    }

    PCB *receiver = searchAndDoStuff(pid, SEARCH);
    if (receiver != NULL)
    {
        size_t msgLen = strlen(msg);
        if (msgLen > MAX_MSG_LEN)
        {
            msgLen = MAX_MSG_LEN;
        }
        char *newMsg = malloc((msgLen + 1) * sizeof(char));
        strncpy(newMsg, msg, msgLen);
        newMsg[msgLen] = '\0';
        List_prepend(receiver->msg, newMsg);
        List_prepend(sendQ, runningProcess);
        runningProcess->state = BLOCK;
        nextProcess();
    }
}
void receive() {}
void reply(int pid, char *msg) {}
void newSemaphore(int sid, int initValue) {}
void P(int sid) {}
void V(int sid) {}
void procInfo(int pid)
{
    searchAndDoStuff(pid, INFO);
}

void totalInfo()
{

    printf("The queues are displayed in right to left order (<-), i.e. the right most pid will be the first one in the queue\n");
    for (int p = 0; p < 3; p++)
    {
        int count = List_count(readyQ[p]);
        if (count > 0)
        {
            List_first(readyQ[p]);
            printf("There are %d pid(s) in ready queue with priority %d: ", count, p);
            printf("%d", ((PCB *)readyQ[p]->current->item)->pid);
            List_next(readyQ[p]);
            for (int i = 0; i < count - 1; i++)
            {
                printf(", %d", ((PCB *)readyQ[p]->current->item)->pid);
                List_next(readyQ[p]);
            }
            printf("\n");
        }
        else
        {
            printf("Ready queue with priority %d is empty\n", p);
        }
    }

    int s = List_count(sendQ);
    int r = List_count(recieveQ);
    if (s > 0)
    {
        List_first(sendQ);
        printf("There are %d pid(s) in send queue: ", s);
        printf("%d", ((PCB *)sendQ->current->item)->pid);
        List_next(sendQ);
        for (int i = 0; i < s - 1; i++)
        {
            printf(", %d", ((PCB *)sendQ->current->item)->pid);
            List_next(sendQ);
        }
        printf("\n");
    }
    else
    {
        printf("send queue is empty\n");
    }

    if (r > 0)
    {
        List_first(recieveQ);
        printf("There are %d pid(s) in send queue: ", r);
        printf("%d ,", ((PCB *)recieveQ->current->item)->pid);
        List_next(recieveQ);
        for (int i = 0; i < r - 1; i++)
        {
            printf(", %d", ((PCB *)recieveQ->current->item)->pid);
            List_next(recieveQ);
        }
        printf("\n");
    }
    else
    {
        printf("Recieve queue is empty\n");
    }
}

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
    init->msg = List_create();
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
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'f':
            fork();
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'k':
            printf("Please enter a pid for the process you want to kill: ");
            scanf("%d", &param1);
            printf("\n");
            exit = kill(param1);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'e':
            exit = exit_();
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'q':
            quantum();
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 's':
            printf("Please enter a pid for the receiving process: ");
            scanf("%d", &param1);
            printf("\n");
            getchar();
            printf("Please enter a message to send: ");
            fgets(msg, MAX_MSG_LEN, stdin);
            printf("\n");
            send(param1, msg);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'r':
            receive();
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'y':
            printf("Please enter a pid for the receiving process: ");
            scanf("%d", &param1);
            printf("\n");
            getchar();
            printf("Please enter a message to reply: ");
            fgets(msg, MAX_MSG_LEN, stdin);
            reply(param1, msg);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'n':
            printf("Please enter a semaphore ID: ");
            scanf("%d", &param1);
            printf("\n");
            printf("Please enter an initial value for the semaphore: ");
            scanf("%d", &param2);
            printf("\n");
            newSemaphore(param1, param2);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'p':
            printf("Please enter a semaphore id: ");
            scanf("%d", &param1);
            printf("\n");
            P(param1);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'v':
            printf("Please enter a semaphore id: ");
            scanf("%d", &param1);
            printf("\n");
            V(param1);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'i':
            printf("Please enter a pid for the receiving process: ");
            scanf("%d", &param1);
            printf("\n");
            procInfo(param1);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 't':
            totalInfo();
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 'm':
            displayMenu();
            break;
        }
    }

    return 0;
}