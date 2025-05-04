#ifndef _UTILS_H
#define _UTILS_H

#include "config.h"

extern FILE* log_file;

#define LOG(message) do { fprintf(log_file, "%s\n", message); } while(0)
void init_logger();
void close_logger();

#endif