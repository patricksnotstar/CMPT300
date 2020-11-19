#include "OSSim.h"

int pid = 0;
static int numProc = 0;
List *readyQ[3];
List *sendQ;
List *receiveQ;
SEM sem[5];

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

void freeMessage(void *pItem)
{
    MSG *temp = pItem;
    free(temp->msg);
    free(temp);
}

bool validateSID(int sid, Code code)
{
    if (sid > 5 || sid < 0)
    {
        printf("Invalid semaphore ID, please try again with a number from 0 to 4\n");
        return false;
    }

    if (code == CREATE)
    {
        if (sem[sid].proc != NULL)
        {
            printf("A semaphore with this ID already exists, please try a different ID\n");
            return false;
        }
    }
    else
    {
        if (sem[sid].proc == NULL)
        {
            printf("A semaphore with this ID does not exist, please try a different ID\n");
            return false;
        }
    }
    return true;
}

void killProcess(PCB *processInQuestion)
{
    int pid = processInQuestion->pid;
    List_free(processInQuestion->msg, freeMessage);
    free(processInQuestion->proc_msg);
    free(processInQuestion);
    numProc--;
    printf("Successfully killed process with pid %d\n", pid);
}

void printMessages(List *messages)
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
        for (int i = 0; i < numMsg; i++)
        {
            MSG *currMSG = messages->current->item;
            printf("    ** from pid: %d: %s\n", currMSG->srcPid, currMSG->msg);
            List_next(messages);
        }
    }
}

int checkReadyQ()
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
    return prio;
}

void nextProcess()
{
    int prio = checkReadyQ();

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
        if (runningProcess->proc_msg != NULL)
        {
            printf("* %s\n", runningProcess->proc_msg);
            free(runningProcess->proc_msg);
            runningProcess->proc_msg = NULL;
        }
    }
}

void printProcInfo(PCB *process)
{
    const char *states[] = {"READY", "BLOCK", "RUNNING"};
    printf("Process with pid %d has: \n", process->pid);
    printf("    * Current state is: %s\n", states[process->state]);
    printf("    * Priority is: %d\n", process->priority);
    printf("    ");
    printMessages(process->msg);
}

PCB *searchAndDoStuff(int pid, Code code)
{
    if (pid == init->pid)
    {
        return init;
    }
    PCB *processInQuestion;
    if (runningProcess->pid == pid)
    {
        if (code == KILL)
        {
            for (int i = 0; i < 5; i++)
            {
                if (runningProcess->sems[i])
                {
                    printf("The semaphore current owned by this process will be released.\n");
                    V(i);
                }
            }
            killProcess(runningProcess);
            nextProcess();
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
                    for (int i = 0; i < 5; i++)
                    {
                        if (processInQuestion->sems[i])
                        {
                            printf("The semaphore current owned by this process will be released.\n");
                            V(i);
                        }
                    }
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
                for (int i = 0; i < 5; i++)
                {
                    if (processInQuestion->sems[i])
                    {
                        printf("The semaphore current owned by this process will be released.\n");
                        V(i);
                    }
                }
                return NULL;
            }
            else if (code == INFO)
            {
                printProcInfo(processInQuestion);
            }
            else if (code == REPLY)
            {
                List_prepend(readyQ[processInQuestion->priority], processInQuestion);
                processInQuestion->state = READY;
                List_remove(sendQ);
            }
            return processInQuestion;
        }
    }
    if (List_count(receiveQ) > 0)
    {
        List_first(receiveQ);
        processInQuestion = List_search(receiveQ, compareInts, &pid);
        if (processInQuestion != NULL)
        {
            if (code == KILL)
            {
                killProcess(processInQuestion);
                List_remove(receiveQ);
                for (int i = 0; i < 5; i++)
                {
                    if (processInQuestion->sems[i])
                    {
                        printf("The semaphore current owned by this process will be released.\n");
                        V(i);
                    }
                }
                return NULL;
            }
            else if (code == INFO)
            {
                printProcInfo(processInQuestion);
            }
            else if (code == SEND)
            {
                List_prepend(readyQ[processInQuestion->priority], processInQuestion);
                processInQuestion->state = READY;
                List_remove(receiveQ);
            }
            return processInQuestion;
        }
    }
    int sid = 0;
    while (sid < 5)
    {
        if (sem[sid].proc == NULL || List_count(sem[sid].proc) == 0)
        {
            sid++;
        }
        else
        {
            List_first(sem[sid].proc);
            processInQuestion = List_search(sem[sid].proc, compareInts, &pid);
            if (processInQuestion == NULL)
            {
                sid++;
            }
            else if (code == KILL)
            {
                killProcess(processInQuestion);
                List_remove(sem[sid].proc);
                for (int i = 0; i < 5; i++)
                {
                    if (processInQuestion->sems[i])
                    {
                        printf("The semaphore current owned by this process will be released.\n");
                        V(i);
                    }
                }
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
    newProcess->proc_msg = NULL;
    for (int i = 0; i < 5; i++)
    {
        newProcess->sems[i] = false;
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
    numProc++;
}

void fork_()
{

    if (runningProcess == init)
    {
        printf("Failed! Cannot fork init!\n");
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
    for (int i = 0; i < 5; i++)
    {
        newProcess->sems[i] = false;
    }

    MSG *original = List_first(runningProcess->msg);
    for (int i = 0; i < List_count(runningProcess->msg); i++)
    {
        MSG *newMSG;
        newMSG = malloc(sizeof(MSG));
        newMSG->srcPid = original->srcPid;
        size_t msgLen = strlen(original->msg);
        newMSG->msg = malloc((msgLen + 1) * sizeof(char));
        strncpy(newMSG->msg, original->msg, msgLen);
        newMSG->msg[msgLen] = '\0';
        List_prepend(newProcess->msg, newMSG);
        original = List_next(runningProcess->msg);
    }

    newProcess->state = READY;
    size_t need = snprintf(NULL, 0, "Forked from process %d. There are %d message(s) waiting to be received.\n", runningProcess->pid, List_count(newProcess->msg));
    newProcess->proc_msg = malloc(need + 1);
    snprintf(newProcess->proc_msg, need, "Forked from process %d. There are %d message(s) waiting to be received.\n", runningProcess->pid, List_count(newProcess->msg));
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
        for (int i = 0; i < 5; i++)
        {
            if (runningProcess->sems[i])
            {
                printf("The semaphore current owned by this process will be released.\n");
                V(i);
            }
        }
        killProcess(runningProcess);
        nextProcess();
        return false;
    }
    else
    {
        if (numProc == 0)
        {

            killProcess(init);
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
        if (checkReadyQ() == 3)
        {
            printf("There are no other ready prcoess so this one will keep running.\n");
        }
        else
        {
            PCB *currentProcess = runningProcess;
            printf("Process %d expired\n", runningProcess->pid);
            nextProcess();
            List_prepend(readyQ[currentProcess->priority], currentProcess);
            currentProcess->state = READY;
        }
    }
    else
    {
        printf("Current running process is init, which does not have a time quantum.\n");
    }
}

void send(int pid, char *msg)
{
    if (runningProcess->pid == pid)
    {
        printf("You cannot send a message to the current running process\n");
        return;
    }

    bool wasOnReceiveQ = false;

    if (List_search(receiveQ, compareInts, &pid) != NULL)
    {
        wasOnReceiveQ = true;
    }

    PCB *receiver = searchAndDoStuff(pid, SEND);
    if (receiver != NULL)
    {
        size_t msgLen = strlen(msg);
        if (msgLen > MAX_MSG_LEN)
        {
            msgLen = MAX_MSG_LEN;
        }

        MSG *newMessage;
        newMessage = malloc(sizeof(MSG));

        newMessage->msg = malloc((msgLen + 1) * sizeof(char));
        strncpy(newMessage->msg, msg, msgLen);
        newMessage->msg[msgLen] = '\0';
        newMessage->srcPid = runningProcess->pid;
        List_prepend(receiver->msg, newMessage);
        if (receiver->proc_msg != NULL)
        {
            free(receiver->proc_msg);
            receiver->proc_msg = NULL;
        }
        size_t need = snprintf(NULL, 0, "Message received from %d, call receive() to see. There are now %d message(s) waiting to be received.\n", runningProcess->pid, List_count(receiver->msg));
        receiver->proc_msg = malloc(need + 1);
        snprintf(receiver->proc_msg, need, "Message received from %d, call receive() to see. There are now %d message(s) waiting to be received.\n", runningProcess->pid, List_count(receiver->msg));
        printf("Sucessfully sent the message\n");

        if (runningProcess->pid != init->pid)
        {
            List_prepend(sendQ, runningProcess);
            runningProcess->state = BLOCK;
            printf("Process %d is now blocked until a reply comes\n", runningProcess->pid);
            nextProcess();
        }
        else if (runningProcess->pid == init->pid && wasOnReceiveQ)
        {
            nextProcess();
        }
    }
}
void receive()
{
    if (List_count(runningProcess->msg) > 0)
    {
        MSG *temp = List_trim(runningProcess->msg);
        printf("Received message from pid %d: %s\n", temp->srcPid, temp->msg);
        free(temp->msg);
        free(temp);
    }
    else if (runningProcess->pid != init->pid)
    {
        List_prepend(receiveQ, runningProcess);
        runningProcess->state = BLOCK;
        printf("No message to receive, blocking until there is a message\n");
        nextProcess();
    }
}

void reply(int pid, char *msg)
{
    if (runningProcess->pid == pid)
    {
        printf("You cannot reply to the current running process\n");
        return;
    }

    bool wasOnSendQ = false;

    if (List_search(sendQ, compareInts, &pid) != NULL)
    {
        wasOnSendQ = true;
    }

    PCB *receiver = searchAndDoStuff(pid, REPLY);
    if (receiver != NULL)
    {
        size_t msgLen = strlen(msg);
        if (msgLen > MAX_MSG_LEN)
        {
            msgLen = MAX_MSG_LEN;
        }

        char *newMessage;
        newMessage = malloc((msgLen + 1) * sizeof(char));

        strncpy(newMessage, msg, msgLen);
        newMessage[msgLen] = '\0';
        if (receiver->proc_msg != NULL)
        {
            free(receiver->proc_msg);
            receiver->proc_msg = NULL;
        }
        size_t need = snprintf(NULL, 0, "There are %d message(s) waiting to be received. Reply received from pid %d, message is: %s\n", List_count(receiver->msg), runningProcess->pid, newMessage);
        receiver->proc_msg = malloc(need + 1);
        snprintf(receiver->proc_msg, need, "There are %d message(s) waiting to be received. Reply received from pid %d, message is: %s\n", List_count(receiver->msg), runningProcess->pid, newMessage);
        free(newMessage);
        printf("Success! Replied to process with pid %d\n", pid);
        if (runningProcess->pid == init->pid && wasOnSendQ)
        {
            nextProcess();
        }
    }
}

void newSemaphore(int sid, int initValue)
{
    if (!validateSID(sid, CREATE))
    {
        return;
    }

    sem[sid].max = initValue;
    sem[sid].curr = initValue;
    sem[sid].proc = List_create();
    printf("Successfully created a semaphore with ID %d\n", sid);
}
void P(int sid)
{
    if (runningProcess->pid == init->pid)
    {
        printf("Init should not own a semaphore, therefore this operation is invalid.\n");
        return;
    }

    if (runningProcess->sems[sid])
    {
        printf("You cannot call P on the same semaphore twice!\n");
        return;
    }

    if (!validateSID(sid, INFO))
    {
        return;
    }

    if (sem[sid].curr > 0)
    {
        printf("Process with pid %d now owns the semaphore\n", runningProcess->pid);
        sem[sid].curr--;
        runningProcess->sems[sid] = true;
        return;
    }

    List_prepend(sem[sid].proc, runningProcess);
    runningProcess->state = BLOCK;
    printf("Process with pid %d has been blocked on semaphore %d\n", runningProcess->pid, sid);
    nextProcess();
}

void V(int sid)
{
    if (runningProcess->pid == init->pid)
    {
        printf("Since Init should not own a semaphore, it would be wrong to call V() so this operation is invalid. \n");
        return;
    }

    if (!runningProcess->sems[sid])
    {
        printf("You cannot a release a semaphore that you don't own\n");
        return;
    }

    if (!validateSID(sid, INFO))
    {
        return;
    }

    if (sem[sid].curr + 1 <= sem[sid].max)
    {
        printf("Success! Semaphore with id %d value is now %d\n", sid, ++sem[sid].curr);
        runningProcess->sems[sid] = false;
    }
    if (sem[sid].curr > 0 && List_count(sem[sid].proc) > 0)
    {
        PCB *wakeyP = List_trim(sem[sid].proc);
        List_prepend(readyQ[wakeyP->priority], wakeyP);
        wakeyP->state = READY;
        printf("Process with pid %d has been woken up and now owns the semaphore with ID %d\n", wakeyP->pid, sid);
    }
}
void procInfo(int pid)
{
    searchAndDoStuff(pid, INFO);
}

void totalInfo()
{

    printf("The queues are displayed in right to left order (<-) or bottom to top order (^), i.e. the right most pid will be the first one in the queue. Please run command t to see all info.\n");
    printf("\n");
    printf("Current running process is %d with priority %d\n", runningProcess->pid, runningProcess->priority);
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
    int r = List_count(receiveQ);
    if (s > 0)
    {
        List_first(sendQ);
        printf("There are %d pid(s) in send queue: \n", s);
        for (int i = 0; i < s; i++)
        {
            printf("    * %d with priority %d\n", ((PCB *)sendQ->current->item)->pid, ((PCB *)sendQ->current->item)->priority);
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
        List_first(receiveQ);
        printf("There are %d pid(s) in receive queue: \n", r);
        for (int i = 0; i < r; i++)
        {
            printf("    * %d with priority %d\n", ((PCB *)receiveQ->current->item)->pid, ((PCB *)receiveQ->current->item)->priority);
            List_next(receiveQ);
        }
        printf("\n");
    }
    else
    {
        printf("Recieve queue is empty\n");
    }

    for (int sid = 0; sid < 5; sid++)
    {
        if (sem[sid].proc != NULL)
        {
            int count = List_count(sem[sid].proc);
            if (count > 0)
            {
                List_first(sem[sid].proc);
                printf("There are %d pid(s) blocked by semaphore with ID %d: \n", count, sid);
                for (int i = 0; i < count; i++)
                {
                    printf("    * %d with priority %d\n", ((PCB *)sem[sid].proc->current->item)->pid, ((PCB *)sem[sid].proc->current->item)->priority);
                    List_next(sem[sid].proc);
                }
                printf("\n");
            }
            else
            {
                printf("There are no processees blocked by semaphore with ID %d\n", sid);
            }
        }
    }
}

void displayMenu()
{
    printf("\n");
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
    // printf("Enter your command here: ");
}

int main()
{
    readyQ[0] = List_create();
    readyQ[1] = List_create();
    readyQ[2] = List_create();
    sendQ = List_create();
    receiveQ = List_create();

    init = malloc(sizeof(PCB));

    init->pid = pid++;
    init->priority = 3;
    init->msg = List_create();
    init->state = RUNNING;
    for (int i = 0; i < 5; i++)
    {
        init->sems[i] = false;
    }
    init->proc_msg = NULL;
    runningProcess = init;

    char command = '\0';
    bool exit = false;

    int param1;
    int param2;
    char msg[MAX_MSG_LEN];
    displayMenu();
    while (!exit)
    {
        if (command == '\0')
        {
            printf("Enter your command here: ");
        }
        scanf("%c", &command);

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
            command = '\0';
            break;
        case 'f':
            fork_();
            printf("-------------------------------------------------------------------------------------\n");
            command = '\0';
            break;
        case 'k':
            printf("Please enter a pid for the process you want to kill: ");
            scanf("%d", &param1);
            printf("\n");
            exit = kill(param1);
            printf("-------------------------------------------------------------------------------------\n");
            command = '\0';
            break;
        case 'e':
            exit = exit_();
            printf("-------------------------------------------------------------------------------------\n");
            command = '\0';
            break;
        case 'q':
            quantum();
            printf("-------------------------------------------------------------------------------------\n");
            command = '\0';
            break;
        case 's':
            printf("Please enter a pid for the receiving process: ");
            scanf("%d", &param1);
            getchar();
            printf("Please enter a message to send: ");
            fgets(msg, MAX_MSG_LEN - 1, stdin);
            send(param1, msg);
            printf("-------------------------------------------------------------------------------------\n");
            command = '\0';
            break;
        case 'r':
            receive();
            printf("-------------------------------------------------------------------------------------\n");
            command = '\0';
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
            command = '\0';
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
            command = '\0';
            break;
        case 'p':
            printf("Please enter a semaphore id: ");
            scanf("%d", &param1);
            printf("\n");
            P(param1);
            printf("-------------------------------------------------------------------------------------\n");
            command = '\0';
            break;
        case 'v':
            printf("Please enter a semaphore id: ");
            scanf("%d", &param1);
            printf("\n");
            V(param1);
            printf("-------------------------------------------------------------------------------------\n");
            command = '\0';
            break;
        case 'i':
            printf("Please enter a pid for the process: ");
            scanf("%d", &param1);
            printf("\n");
            procInfo(param1);
            printf("-------------------------------------------------------------------------------------\n");
            command = '\0';
            break;
        case 't':
            totalInfo();
            printf("-------------------------------------------------------------------------------------\n");
            command = '\0';
            break;
        case 'm':
            displayMenu();
            command = '\0';
            break;
        case '\n':
            break;
        default:
            printf("Invalid command\n");
            command = '\0';
        }
    }

    return 0;
}