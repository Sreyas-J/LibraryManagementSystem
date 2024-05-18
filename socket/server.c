#include "../UserAuthentication/profile.h"
#include "../admin/member.h"
#include "../admin/book.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include<fcntl.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

void *clientHandler(void *socket_desc);
Profile *admin;

int main() {
    FILE *fp = fopen(profilesDB, "w");

    int fd = fileno(fp);
    pthread_mutex_lock(&mutex);
    lockFile(fd, F_WRLCK);

    fprintf(fp, "ID,Name,Password,Admin Status,No. of books Borrowed\n");  // Write a newline character to the file

    lockFile(fd, F_UNLCK);
    pthread_mutex_unlock(&mutex);
    fclose(fp);

    FILE *f = fopen(booksDB, "w");
    fd=fileno(f);
    pthread_mutex_lock(&Bookmutex);
    lockFile(fd, F_WRLCK);

    fprintf(f, "ID,Title,Author,Copies\n");  // Write a newline character to the file

    lockFile(fd, F_UNLCK);
    pthread_mutex_unlock(&Bookmutex);
    fclose(f);

    FILE *f1 = fopen(transactionsDB, "w");
    fd=fileno(f1);
    pthread_mutex_lock(&Transactionmutex);
    lockFile(fd, F_WRLCK);

    fprintf(f1, "TransactionID,ProfileID,BookID,Copies,Transaction Type\n");  // Write a newline character to the file

    lockFile(fd, F_UNLCK);
    pthread_mutex_unlock(&Transactionmutex);
    fclose(f1);

    createProfile("admin", 1, "admin");

    int server_fd, client_socket, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;
    int opt = 1;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Could not create socket\n");
        return 1;
    }
    printf("Socket created\n");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }
    printf("Bind done\n");

    // Listen
    listen(server_fd, MAX_CLIENTS);

    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);

    // Accept and incoming connection
    while ((client_socket = accept(server_fd, (struct sockaddr *)&client, (socklen_t*)&c))) {
        printf("Connection accepted\n");

        if (pthread_create(&thread_id, NULL, clientHandler, (void*)&client_socket) < 0) {
            perror("Could not create thread");
            return 1;
        }

        printf("Handler assigned\n");
    }

    if (client_socket < 0) {
        perror("Accept failed");
        return 1;
    }

    return 0;
}

void *clientHandler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    int read_size;
    int state=1;
    char client_message[BUFFER_SIZE];
    char name[BUFFER_SIZE], password[BUFFER_SIZE];
    char prompt[BUFFER_SIZE];

    strcpy(prompt, "Login/SignUp:\n");
    write(sock, prompt, strlen(prompt));
    
    memset(client_message, 0, BUFFER_SIZE);

    Profile *profile=NULL;

    read_size = recv(sock, client_message, BUFFER_SIZE, 0);
    if (read_size <= 0) {
        close(sock);
        return 0;
    }
    client_message[read_size - 1] = '\0'; // Remove newline character
    if(strcmp(client_message, "SignUp") == 0) state=0;
    while(profile==NULL){
        while (1) {
            strcpy(prompt, "Enter a valid username:\n");
            write(sock, prompt, strlen(prompt));
            memset(client_message, 0, BUFFER_SIZE);
            read_size = recv(sock, client_message, BUFFER_SIZE, 0);
            client_message[read_size - 1] = '\0'; // Remove newline character
            if (strlen(client_message) > 0) {
                strcpy(name, client_message);
                break;
            }
        }

        while (1) {
            strcpy(prompt, "Enter a valid password:\n");
            write(sock, prompt, strlen(prompt));
            memset(client_message, 0, BUFFER_SIZE);
            read_size = recv(sock, client_message, BUFFER_SIZE, 0);
            client_message[read_size - 1] = '\0'; // Remove newline character
            if (strlen(client_message) > 0) {
                strcpy(password, client_message);
                break;
            }
        }

        if (state==0) {
            // SignUp process
            createProfile(name, 0, password);
        }

        profile = login(name,0,password);
    }

    strcpy(prompt, "User logged in!\n Borrow books (BORROW)\n Return books (RETURN)\n View Profile (VIEW)\n");
    
    while(1){
        write(sock, prompt, strlen(prompt));
        memset(client_message, 0, BUFFER_SIZE);

        read_size = recv(sock, client_message, BUFFER_SIZE, 0);
        client_message[read_size - 1] = '\0';

        if(strlen(client_message) > 0){
            if(strcmp(client_message,"BORROW")==0 || strcmp(client_message,"RETURN")==0 || strcmp(client_message,"VIEW")==0) break;
        }
        strcpy(prompt,"Enter a valid input\n");
    }
    
    if(strcmp(client_message,"VIEW")==0){
        searchMember(profile,profile->name,prompt);
        write(sock, prompt, strlen(prompt));
        memset(client_message, 0, BUFFER_SIZE);
    }

    else if(strcmp(client_message,"BORROW")==0){
        // borrowBook();
    }

    // Free the socket pointer
    close(sock);
    return 0;
}
