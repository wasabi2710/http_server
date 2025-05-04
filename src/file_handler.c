#include "file_handler.h"

char* stream_file(const char path[], UINT* content_len) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        LOG("Failed to open file");
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        LOG("fseek failed");
        return NULL;
    }

    long file_size = ftell(file);
    if (file_size == -1L) {
        fclose(file);
        LOG("ftell failed");
        return NULL;
    }

    rewind(file);

    char* buffer = malloc(file_size);
    if (!buffer) {
        fclose(file);
        LOG("Memory allocation failed for file buffer");
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if (bytes_read != (size_t)file_size) {
        free(buffer);
        LOG("Incomplete file read");
        return NULL;
    }

    *content_len = (UINT)file_size;

    char log_buf[128];
    snprintf(log_buf, sizeof(log_buf), "Streamed file (%u bytes)", *content_len);
    LOG(log_buf);

    return buffer;
}