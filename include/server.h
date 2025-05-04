#ifndef _SERVER_C
#define _SERVER_C

#include "config.h"
#include "utils.h"

// server modules
#include "response.h"

typedef struct {
    char ip[INET_ADDRSTRLEN];
    u_short port;
} IpPort;

IpPort extract_ip_port(struct sockaddr_in addrl);
void server();

#endif