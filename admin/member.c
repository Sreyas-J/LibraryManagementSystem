#include "member.h"
#include "../supportFunctions/support.h"
#include "../UserAuthentication/profile.h"
#include "../admin/book.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>


pthread_mutex_t Transactionmutex = PTHREAD_MUTEX_INITIALIZER;


typedef struct {
    int *bookIDs;
    int count;
    int *copies;
} BookList;


BookList* getBookIDsForProfile(int profileID) {
    FILE *fp = fopen(transactionsDB, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    int fd = fileno(fp);  // Get file descriptor associated with the FILE stream

    // Lock the critical section using Bookmutex
    pthread_mutex_lock(&Transactionmutex);

    // Acquire write lock
    lockFile(fd, F_RDLCK);

    char line[MAX_SIZE * 5];
    BookList *bookList = malloc(sizeof(BookList));
    if (bookList == NULL) {
        printf("Memory allocation failed!\n");
        fclose(fp);
        return NULL;
    }

    bookList->bookIDs = malloc(MAX_SIZE * sizeof(int));  // Initial allocation for 100 book IDs
    bookList->copies = malloc(MAX_SIZE * sizeof(int));
    if (bookList->bookIDs == NULL) {
        printf("Memory allocation failed!\n");
        free(bookList);
        fclose(fp);
        return NULL;
    }

    bookList->count = 0;
    Transaction *transaction=malloc(sizeof(Transaction));

    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%d,%d,%d,%d,%[^,\n]", &transaction->transactionID, &transaction->profileID, &transaction->bookID, &transaction->copies, transaction->type);

        if (transaction->profileID == profileID) {
            if (bookList->count >= MAX_SIZE) {
                // Reallocate if the current allocation is not enough
                bookList->bookIDs = realloc(bookList->bookIDs, (bookList->count + MAX_SIZE) * sizeof(int));
                if (bookList->bookIDs == NULL) {
                    printf("Memory allocation failed!\n");
                    free(bookList);
                    fclose(fp);
                    return NULL;
                }
            }
            bookList->bookIDs[bookList->count] = transaction->bookID;
            bookList->copies[bookList->count]= transaction->copies;
            bookList->count++;
        }
    }

    lockFile(fd, F_UNLCK);

    // Unlock the critical section
    pthread_mutex_unlock(&Transactionmutex);

    fclose(fp);
    return bookList;
}


void printBookDetails(BookList *booklist,char str[]) {
    FILE *fp = fopen(booksDB, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    int fd = fileno(fp);  // Get file descriptor associated with the FILE stream

    // Lock the critical section using Bookmutex
    pthread_mutex_lock(&Bookmutex);

    // Acquire write lock
    lockFile(fd, F_RDLCK);


    char line[MAX_SIZE * 4];
    Book book;
    int *foundBooks = calloc(booklist->count, sizeof(int));

    if (foundBooks == NULL) {
        printf("Memory allocation failed!\n");
        fclose(fp);
        return;
    }
    char msg[]="";

    fgets(line, sizeof(line), fp);
    printf("Books borrowed:\n");
    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%d,%[^,],%[^,],%d", &book.id, book.title, book.author, &book.copies);

        for (int i = 0; i < booklist->count; i++) {
            if (book.id == booklist->bookIDs[i] && !foundBooks[i]) {

                sprintf(msg,"ID: %d, Title: %s, Author: %s, Copies: %d\n", book.id, book.title, book.author,booklist->copies[i]);
                strcat(str,msg);
                foundBooks[i] = 1; // Mark this book ID as found
                break;
            }
        }
    }

    lockFile(fd, F_UNLCK);

    // Unlock the critical section
    pthread_mutex_unlock(&Bookmutex);
    fclose(fp);

    free(foundBooks);
}


void listMembers(Profile *profile){
    if(profile->admin==1){
        readAndUpdateProfiles(profile->name,profile->admin,profile->password,0,1);
    }
    else{
        printf("User doesn't have the required permissions.\n");
    }
}


void searchMember(Profile *profile,char Name[],char str[]){
    BookList *booklist;
    if(profile->admin==1){
        printf("User details:-\n");
        strcpy(str,"User details:-\n");
        Profile *customer=readAndUpdateProfiles(Name,0,"",0,3);
        booklist=getBookIDsForProfile(customer->id);
    }
    else if(strcmp(profile->name,Name)==0){
        printf("User details:-\n");
        strcpy(str,"User details:-\n");
        booklist=getBookIDsForProfile(profile->id);
    }
    else{
        printf("This user doesn't have permission to access this data\n");
        return;
    }
    printBookDetails(booklist,str);
}