#ifndef _NETWORK_H
#define _NETWORK_H
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>

typedef struct Network NetworkInfo;
struct Network
{
    struct addrinfo *res;
    int local_socketDescriptor;
};

#endif