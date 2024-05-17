#ifndef SUPPORT_H
#define SUPPORT_H
#include <pthread.h>  

#define MAX_SIZE 20

extern pthread_mutex_t Bookmutex;
extern pthread_mutex_t Transactionmutex;
extern pthread_mutex_t mutex;

extern int nextProfileId;  
extern int nextTransactionId;
extern int nextBookId;
extern char profilesDB[];
extern char booksDB[];
extern char transactionsDB[];

typedef struct Book {
    int id;
    int copies;
    char title[MAX_SIZE];
    char author[MAX_SIZE];
} Book;

typedef struct Profile {
    int id;
    char name[MAX_SIZE];
    // int books[MAX_SIZE];
    char password[MAX_SIZE];
    int borrowed;
    int admin;
} Profile;

typedef struct Transaction {
    int transactionID;
    int profileID;
    int bookID;
    int copies;
    char type[];
} Transaction;

void lockFile(int fd, int lockType);

#endif