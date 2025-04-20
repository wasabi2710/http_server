#include "parser.h"
#include "logger.h"
#include "config.h"

typedef struct {
    char ip[INET_ADDRSTRLEN];  // 16 bytes (IPv4 max)
    u_short port;              // 2 bytes
} IpPort;

IpPort extract_ip_port(struct sockaddr_in addr) {
    IpPort result;
    inet_ntop(AF_INET, &addr.sin_addr, result.ip, sizeof(result.ip));
    result.port = ntohs(addr.sin_port);
    return result;
}

int main() {    
    init_logger(); //start logger..

    WSADATA wsaData = {0};
    SOCKET sockfd = 0;
    SOCKET clientfd = 0;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(3, 3), &wsaData) < 0) 
        LOGERROR("Failed Init WSAData");

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) 
        LOGERROR("Failed Socket Init");

    // Bind
    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) 
        LOGERROR("Failed binding the server socket");

    // Listen
    if (listen(sockfd, SOMAXCONN)) 
        LOGERROR("Failed Setting Listener");

    printf("Server listening on port %d...\n", SERVER_PORT);

    while(1) {

        struct sockaddr_in clientAddr = {0}; 
        int clientAddrLen = sizeof(clientAddr);

        clientfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientfd == INVALID_SOCKET) {
            printf("Failed accepting client: %d\n", WSAGetLastError());
            continue;
        }

        IpPort ipPort = extract_ip_port(clientAddr);

        //logger
        //clients
        printf("Client: %s:%d\n", ipPort.ip, ipPort.port);
        // if i were to calse func parse_http_header here
        //HttpRequest request = parse_http_request(clientfd);

        // file server
        file_server(clientfd);
        
        closesocket(clientfd);

    }

    closesocket(sockfd);
    WSACleanup();
    close_logger();
    return 0;
}