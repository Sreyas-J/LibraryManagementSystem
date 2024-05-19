#include "../UserAuthentication/profile.h"
#include "../admin/member.h"
#include "../admin/book.h"
#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include<fcntl.h>

#define PORT 8080
#define MAX_CLIENTS 100


void *clientHandler(void *socket_desc);
Profile *admin;
int sock,read_size;


void sendToClient(char msg[],char prompt[],char client_message[],char var[]){
    while (1) {
        strcpy(prompt, msg);
        write(sock, prompt, strlen(prompt));
        memset(client_message, 0, BUFFER_SIZE);
        read_size = recv(sock, client_message, BUFFER_SIZE, 0);
        client_message[read_size - 1] = '\0'; // Remove newline character
        if (strlen(client_message) > 0) {
            strcpy(var, client_message);
            break;
        }
    }
}


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
    sock = *(int*)socket_desc;
    int state=1;
    char client_message[BUFFER_SIZE];
    char name[BUFFER_SIZE], password[BUFFER_SIZE];
    char prompt[BUFFER_SIZE];

    while(1){
        sendToClient("Login/SignUp:\n",prompt,client_message,client_message);
        if(strcmp(client_message,"SignUp")==0){
            state=0;
            break;
        }
        else if(strcmp(client_message,"Login")==0) break;
    }

    Profile *profile=NULL;

    while(profile==NULL){
        sendToClient("Enter a valid username:\n",prompt,client_message,name);
        sendToClient("Enter a valid password:\n",prompt,client_message,password);

        if (state==0) {
            // SignUp process
            createProfile(name, 0, password);
        }

        profile = login(name,password);
    }

    
    if(profile->admin==0){
        while(1){
            sendToClient("MENU:-\n Borrow books (BORROW)\n Return books (RETURN)\n View Profile (VIEW)\n",prompt,client_message,client_message);
            if(strcmp(client_message,"BORROW")==0 || strcmp(client_message,"RETURN")==0 || strcmp(client_message,"VIEW")==0) break;
        }
        
        if(strcmp(client_message,"VIEW")==0){
            searchMember(profile,profile->name,prompt);
            write(sock, prompt, strlen(prompt));
        }

        else if(strcmp(client_message,"BORROW")==0){
            memset(prompt,0,BUFFER_SIZE);
            printBooks(prompt);
            write(sock, prompt, strlen(prompt));
        }

        memset(client_message, 0, BUFFER_SIZE);
    }
    else{
        sendToClient("MENU:-\n Add books (ADDbook)\n Search books (SEARCHbook)\n Modify books (UPDATEbook)\n Delete books (DELETEbook)\n List books (LISTbooks)\n Search members (SEARCHmember)\n List members (LISTmembers)\n",prompt,client_message,client_message);
        
        if(strcmp(client_message,"ADDbook")==0){
            sendToClient("Kindly enter a valid title of book.\n",prompt,client_message,name);
            sendToClient("Kindly enter the valid author name of the book.\n",prompt,client_message,password);
            sendToClient("Kindly enter the no. of copies added.\n",prompt,client_message,client_message);

            Book *book=addBook(name,password,profile,atoi(client_message));
        }
    }
    // Free the socket pointer
    close(sock);
    return 0;
}
