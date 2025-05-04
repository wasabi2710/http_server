#ifndef _CONFIG_H
#define _CONFIG_H

// external modules
#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <minwindef.h>

// server definitions
#define HTTP_HEADER_END "\r\n\r\n"
#define SERVER_PORT 8000
#define MAX_HEADER_SIZE 4096
#define METHOD_SIZE 8
#define MAX_PATH_SIZE 1024

#endif