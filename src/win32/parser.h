#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"
#include "config.h"
#include "logger.h"

typedef struct {
    char method[METHOD_SIZE];
    char path[MAX_PATH_SIZE];
} HttpRequest;

char* stream_file(const char path[], UINT* content_len);
HttpRequest parse_http_request(SOCKET clientfd);
void file_server(SOCKET clientfd);
char* get_mime_type(const char* requested_file);
char* normalize_path(char* path);
char* response_construction(int code, UINT content_len, const char* mime_type);

#endif