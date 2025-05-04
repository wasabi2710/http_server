#ifndef _REQUEST_H
#define _REQUEST_H

#include "config.h"
#include "utils.h"

typedef struct {
    char method[METHOD_SIZE];
    char path[MAX_PATH_SIZE];
} HttpRequest;

HttpRequest parse_http_request(SOCKET clientfd);

#endif