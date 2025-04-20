#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdio.h>
#include <stdlib.h>

extern FILE* log_file;

#define LOGERROR(err) do { fprintf(log_file, "%s\n", err); } while(0)
void init_logger();
void close_logger();

#endif