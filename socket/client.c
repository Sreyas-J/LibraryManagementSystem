#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

    memset(server_reply, 0, BUFFER_SIZE);

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket\n");
        return 1;
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed. Error");
        return 1;
    }

    printf("Connected\n");

    // Keep communicating with server
    while (1) {
        if (recv(sock, server_reply, BUFFER_SIZE, 0) < 0) {
            printf("Recv failed\n");
            break;
        }

        printf("%s",server_reply);
        fgets(message, BUFFER_SIZE, stdin);

        // Send some data
        if (send(sock, message, strlen(message), 0) < 0) {
            printf("Send failed\n");
            return 1;
        }
        memset(server_reply, 0, BUFFER_SIZE);
    }

    close(sock);
    return 0;
}
