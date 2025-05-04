#include "server.h"

// helpers
// extract addr
IpPort extract_ip_port(struct sockaddr_in addr) {
    IpPort result;
    inet_ntop(AF_INET, &addr.sin_addr, result.ip, sizeof(result.ip));
    result.port = ntohs(addr.sin_port);
    return result;
}

// main server loop
void server() {
    WSADATA wsaData = {0};
    SOCKET sockfd = 0;
    SOCKET clientfd = 0;

    if (WSAStartup(MAKEWORD(3, 3), &wsaData) < 0) 
        LOG("Failed init wsadat\n");

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) 
        LOG("Failed Socket Init");

    // Bind
    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) 
        LOG("Failed binding the server socket");

    // Listen
    if (listen(sockfd, SOMAXCONN)) 
        LOG("Failed Setting Listener");

    printf("Server listening on port %d...\n", SERVER_PORT);

    while(1) {

        struct sockaddr_in clientAddr = {0}; 
        int clientAddrLen = sizeof(clientAddr);

        clientfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientfd == INVALID_SOCKET) {
            printf("Failed accepting client: %d\n", WSAGetLastError());
            continue;
        }

        //
        response(clientfd);
        
        closesocket(clientfd);
    }

    closesocket(sockfd);
    WSACleanup();
    close_logger();
}
