#include "parser.h"
#include "common.h"
#include "config.h"
#include "logger.h"

void file_server(SOCKET clientfd) {
    HttpRequest requested_header = {0};
    const char* requested_path = NULL;
    char* file_buffer = NULL;
    char* mime_type = NULL;
    char* response_buffer = NULL;
    UINT response_buffer_len = 0;
    UINT content_len = 0;

    LOGERROR("Handling new client request...");

    requested_header = parse_http_request(clientfd);

    requested_path = normalize_path(requested_header.path);
    char log_buf[256];
    snprintf(log_buf, sizeof(log_buf), "Requested path: %s", requested_path);
    LOGERROR(log_buf);

    file_buffer = stream_file(requested_path, &content_len);
    if (!file_buffer) {
        LOGERROR("File not found or failed to stream file");

        response_buffer = response_construction(404, 0, NULL);
        response_buffer_len = strlen(response_buffer);
        send(clientfd, response_buffer, response_buffer_len, 0);
        free(response_buffer);
        closesocket(clientfd);
        return;
    }

    mime_type = get_mime_type(requested_path);
    snprintf(log_buf, sizeof(log_buf), "MIME type: %s", mime_type);
    LOGERROR(log_buf);

    response_buffer = response_construction(200, content_len, mime_type);
    response_buffer_len = strlen(response_buffer);

    if (send(clientfd, response_buffer, response_buffer_len, 0) <= 0) {
        LOGERROR("Failed to send headers to client");
    } else if (send(clientfd, file_buffer, content_len, 0) <= 0) {
        LOGERROR("Failed to send file content to client");
    } else {
        snprintf(log_buf, sizeof(log_buf), "Served file: %s (%u bytes)", requested_path, content_len);
        LOGERROR(log_buf);
    }

    free(file_buffer);
    free(response_buffer);
    closesocket(clientfd);
    LOGERROR("Connection closed");
}

char* get_mime_type(const char* requested_file) {
    const char* ext = strrchr(requested_file, '.'); 
    if (!ext) return "application/octet-stream";
    ext++;

    char log_buf[128];
    snprintf(log_buf, sizeof(log_buf), "File extension: %s", ext);
    LOGERROR(log_buf);

    if (strcmp(ext, "html") == 0) return "text/html";
    if (strcmp(ext, "css") == 0) return "text/css";
    if (strcmp(ext, "js") == 0) return "application/javascript";
    if (strcmp(ext, "png") == 0) return "image/png";
    if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, "txt") == 0) return "text/plain";
    if (strcmp(ext, "pdf") == 0) return "application/pdf";

    return "application/octet-stream";
}

char* normalize_path(char* path) {
    if (strcmp(path, "/") == 0) {
        return "index.html"; 
    }
    if (path[0] == '/') {
        return path + 1; 
    }
    return path;
}

char* response_construction(int code, UINT content_len, const char* mime_type) {
    const char* header_template_200 =
        "HTTP/1.1 %d OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %u\r\n"
        "Connection: Close\r\n"
        "\r\n";

    const char* header_template_404 =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Length: 0\r\n"
        "Connection: Close\r\n"
        "\r\n";

    char* response;

    if (code == 200) {
        int len = snprintf(NULL, 0, header_template_200, code, mime_type, content_len);
        response = malloc(len + 1);
        if (!response) {
            LOGERROR("Failed allocating memory for 200 response");
            return NULL;
        }
        sprintf(response, header_template_200, code, mime_type, content_len);
    } else {
        response = strdup(header_template_404);
        if (!response) {
            LOGERROR("Failed allocating memory for 404 response");
            return NULL;
        }
    }

    LOGERROR("Constructed HTTP response headers");
    return response;
}

HttpRequest parse_http_request(SOCKET clientfd) {
    HttpRequest request = {0};
    char parsed_content[MAX_HEADER_SIZE];

    int received = recv(clientfd, parsed_content, MAX_HEADER_SIZE - 1, 0);
    if (received <= 0) {
        LOGERROR("Failed to receive HTTP header");
        return request;
    }

    parsed_content[received] = '\0';

    if ((size_t)received > MAX_HEADER_SIZE && memcmp(parsed_content + received - 4, HTTP_HEADER_END, 4) != 0) {
        LOGERROR("Incomplete or invalid HTTP header");
    }

    const char* compart = parsed_content;
    while (*compart && !(compart[0] == '\r' && compart[1] == '\n')) {
        compart++;
    }
    size_t compart_len = compart - parsed_content;

    char first_line[compart_len + 1];
    memcpy(first_line, parsed_content, compart_len);
    first_line[compart_len] = '\0';

    sscanf(first_line, "%s %s", request.method, request.path);

    char log_buf[256];
    snprintf(log_buf, sizeof(log_buf), "Parsed HTTP request: method=%s path=%s", request.method, request.path);
    LOGERROR(log_buf);

    return request;
}

char* stream_file(const char path[], UINT* content_len) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        LOGERROR("Failed to open file");
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        LOGERROR("fseek failed");
        return NULL;
    }

    long file_size = ftell(file);
    if (file_size == -1L) {
        fclose(file);
        LOGERROR("ftell failed");
        return NULL;
    }

    rewind(file);

    char* buffer = malloc(file_size);
    if (!buffer) {
        fclose(file);
        LOGERROR("Memory allocation failed for file buffer");
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if (bytes_read != (size_t)file_size) {
        free(buffer);
        LOGERROR("Incomplete file read");
        return NULL;
    }

    *content_len = (UINT)file_size;

    char log_buf[128];
    snprintf(log_buf, sizeof(log_buf), "Streamed file (%u bytes)", *content_len);
    LOGERROR(log_buf);

    return buffer;
}
