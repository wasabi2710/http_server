#include "response.h"

/**
 * @brief http response parser
 * 
 * @param clientfd 
 */
void response(SOCKET clientfd) {
    HttpRequest requested_header = {0};
    const char* requested_path = NULL;
    char* file_buffer = NULL;
    char* mime_type = NULL;
    char* response_buffer = NULL;
    UINT response_buffer_len = 0;
    UINT content_len = 0;

    LOG("Handling new client request...");

    requested_header = parse_http_request(clientfd);

    requested_path = normalize_path(requested_header.path);
    char log_buf[256];
    snprintf(log_buf, sizeof(log_buf), "Requested path: %s", requested_path);
    LOG(log_buf);

    file_buffer = stream_file(requested_path, &content_len);
    if (!file_buffer) {
        LOG("File not found or failed to stream file");

        response_buffer = response_construction(404, 0, NULL);
        response_buffer_len = strlen(response_buffer);
        send(clientfd, response_buffer, response_buffer_len, 0);
        free(response_buffer);
        closesocket(clientfd);
        return;
    }

    mime_type = get_mime_type(requested_path);
    snprintf(log_buf, sizeof(log_buf), "MIME type: %s", mime_type);
    LOG(log_buf);

    response_buffer = response_construction(200, content_len, mime_type);
    response_buffer_len = strlen(response_buffer);

    if (send(clientfd, response_buffer, response_buffer_len, 0) <= 0) {
        LOG("Failed to send headers to client");
    } else if (send(clientfd, file_buffer, content_len, 0) <= 0) {
        LOG("Failed to send file content to client");
    } else {
        snprintf(log_buf, sizeof(log_buf), "Served file: %s (%u bytes)", requested_path, content_len);
        LOG(log_buf);
    }

    free(file_buffer);
    free(response_buffer);
    closesocket(clientfd);
    LOG("Connection closed");
}

/**
 * @brief get requested path mime type
 * 
 * @param requested_file 
 * @return char* 
 */
char* get_mime_type(const char* requested_file) {
    const char* ext = strrchr(requested_file, '.'); 
    if (!ext) return "application/octet-stream";
    ext++;

    char log_buf[128];
    snprintf(log_buf, sizeof(log_buf), "File extension: %s", ext);
    LOG(log_buf);

    if (strcmp(ext, "html") == 0) return "text/html";
    if (strcmp(ext, "css") == 0) return "text/css";
    if (strcmp(ext, "js") == 0) return "application/javascript";
    if (strcmp(ext, "png") == 0) return "image/png";
    if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, "txt") == 0) return "text/plain";
    if (strcmp(ext, "pdf") == 0) return "application/pdf";

    return "application/octet-stream";
}

/**
 * @brief response constructor for http response parser
 * 
 * @param code 
 * @param content_len 
 * @param mime_type 
 * @return char* 
 */
char* response_construction(int code, UINT content_len, const char* mime_type) {
    const char* header_template_200 =
        "HTTP/1.1 %d OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %u\r\n"
        "Connection: Close\r\n"
        // Security Headers
        "Strict-Transport-Security: max-age=31536000; includeSubDomains\r\n"
        "Content-Security-Policy: default-src 'self'; script-src 'self'; object-src 'none';\r\n"
        "X-Content-Type-Options: nosniff\r\n"
        "X-Frame-Options: SAMEORIGIN\r\n"
        "Referrer-Policy: no-referrer\r\n"
        "\r\n";

    const char* header_template_404 =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Length: 0\r\n"
        "Connection: Close\r\n"
        // Security Headers
        "Strict-Transport-Security: max-age=31536000; includeSubDomains\r\n"
        "Content-Security-Policy: default-src 'self'; script-src 'self'; object-src 'none';\r\n"
        "X-Content-Type-Options: nosniff\r\n"
        "X-Frame-Options: SAMEORIGIN\r\n"
        "Referrer-Policy: no-referrer\r\n"
        "\r\n";

    char* response;

    if (code == 200) {
        int len = snprintf(NULL, 0, header_template_200, code, mime_type, content_len);
        response = malloc(len + 1);
        if (!response) {
            LOG("Failed allocating memory for 200 response");
            return NULL;
        }
        sprintf(response, header_template_200, code, mime_type, content_len);
    } else {
        response = strdup(header_template_404);
        if (!response) {
            LOG("Failed allocating memory for 404 response");
            return NULL;
        }
    }

    LOG("Constructed HTTP response headers");
    return response;
}