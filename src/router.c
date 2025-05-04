#include "router.h"

/**
 * @brief normalize index
 * 
 * @param path 
 * @return char* 
 */
char* normalize_path(char* path) {
    const char* filename;
    if (strcmp(path, "/") == 0) {
        filename = "index.html";
    } else if (path[0] == '/') {
        filename = path + 1;
    } else {
        filename = path;
    }

    // Build the full path: public/filename
    const char* root = "public/";
    size_t len = strlen(root) + strlen(filename) + 1;
    char* full_path = malloc(len);
    if (!full_path) return NULL;

    snprintf(full_path, len, "%s%s", root, filename);
    return full_path; 
}
