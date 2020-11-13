#ifndef _OSSIM_H_
#define _OSSIM_H_

#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_MSG_LEN 40

typedef struct PCB PCB;

enum State
{
    READY,
    BLOCK,
    RUNNING
};

struct PCB
{
    int pid;
    int priority;
    enum State state;
    List *msg;
};

typedef struct SEM Sem;
struct SEM
{
    int value;
    List *proc;
};

void create(int);
void fork();
void kill(int);
void exit_();
void quantum();
void send(int, char *);
void receive();
void reply(int, char *);
void newSemaphore(int, int);
void P(int);
void V(int);
void procInfo(int);
void totalInfo();
#endif