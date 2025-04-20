#include "logger.h"

FILE* log_file = NULL; 

void init_logger() {
    log_file = fopen("_sys_log.txt", "a");
    if (!log_file) {
        perror("Failed to open log file");
    }
}

void close_logger() {
    if (log_file) {
        fclose(log_file);
    }
}