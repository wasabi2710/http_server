#include "server.h"

int main() {
    init_logger();

    server(); // server

    close_logger();
    return 0;
}