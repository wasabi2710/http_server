#include "request.h"

/**
 * @brief request parser
 * 
 * @param clientfd 
 * @return HttpRequest 
 */
HttpRequest parse_http_request(SOCKET clientfd) {
    HttpRequest request = {0};
    char parsed_content[MAX_HEADER_SIZE];

    int received = recv(clientfd, parsed_content, MAX_HEADER_SIZE - 1, 0);
    if (received <= 0) {
        LOG("Failed to receive HTTP header");
        return request;
    }

    parsed_content[received] = '\0';

    if ((size_t)received > MAX_HEADER_SIZE && memcmp(parsed_content + received - 4, HTTP_HEADER_END, 4) != 0) {
        LOG("Incomplete or invalid HTTP header");
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
    LOG(log_buf);

    return request;
}