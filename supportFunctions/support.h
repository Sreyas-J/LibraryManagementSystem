#ifndef SUPPORT_H
#define SUPPORT_H
#include <pthread.h>  

#define MAX_SIZE 20

extern int nextProfileId;  
extern int nextBookId;
extern char profilesDB[];
extern char booksDB[];

typedef struct Book {
    int id;
    int copies;
    char title[MAX_SIZE];
    char author[MAX_SIZE];
} Book;

typedef struct Profile {
    int id;
    char name[MAX_SIZE];
    int books[MAX_SIZE];
    char password[MAX_SIZE];
    int borrowed;
    int admin;
} Profile;

void lockFile(int fd, int lockType);

#endif