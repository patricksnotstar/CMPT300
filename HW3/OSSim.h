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
    char *proc_msg;
    bool sems[5];
};

typedef struct SEM SEM;
struct SEM
{
    int max;
    int curr;
    List *proc;
};

typedef struct MSG MSG;
struct MSG
{
    char *msg;
    int srcPid;
};

typedef enum
{
    KILL,
    INFO,
    SEARCH,
    NEXT,
    SEND,
    REPLY,
    CREATE
} Code;

void create(int);
void fork_();
bool kill(int);
bool exit_();
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