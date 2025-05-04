#ifndef _RESPONSE_H
#define _RESPONSE_H

// server modules
#include "config.h"
#include "utils.h"
#include "request.h"
#include "file_handler.h"
#include "router.h"

void response(SOCKET clientfd);
char* get_mime_type(const char* requested_file);
char* response_construction(int code, UINT content_len, const char* mime_type);

#endif