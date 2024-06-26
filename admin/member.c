#include "member.h"
#include "../supportFunctions/support.h"
#include "../UserAuthentication/profile.h"
#include "../admin/book.h"
#include "../socket/server.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>


pthread_mutex_t Transactionmutex = PTHREAD_MUTEX_INITIALIZER;


typedef struct {
    int *bookIDs;
    int *borrowedCopies;
    int *returnedCopies;
    int count;
} BookList;



BookList* getBookIDsForProfile(int profileID) {
    FILE *fp = fopen(transactionsDB, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    int fd = fileno(fp);  // Get file descriptor associated with the FILE stream

    // Lock the critical section using Transactionmutex
    pthread_mutex_lock(&Transactionmutex);

    // Acquire read lock
    lockFile(fd, F_RDLCK);

    char line[MAX_SIZE * 5];
    BookList *bookList = malloc(sizeof(BookList));
    if (bookList == NULL) {
        printf("Memory allocation failed!\n");
        fclose(fp);
        return NULL;
    }

    bookList->bookIDs = malloc(MAX_SIZE * sizeof(int));  // Initial allocation for 100 book IDs
    bookList->borrowedCopies = malloc(MAX_SIZE * sizeof(int));
    bookList->returnedCopies = malloc(MAX_SIZE * sizeof(int));

    if (bookList->bookIDs == NULL || bookList->borrowedCopies == NULL || bookList->returnedCopies == NULL) {
        printf("Memory allocation failed!\n");
        free(bookList->bookIDs);
        free(bookList->borrowedCopies);
        free(bookList->returnedCopies);
        free(bookList);
        fclose(fp);
        return NULL;
    }

    bookList->count = 0;
    Transaction transaction;

    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%d,%d,%d,%d,%[^,\n]", &transaction.transactionID, &transaction.profileID, &transaction.bookID, &transaction.copies, transaction.type);

        if (transaction.profileID == profileID) {
            // Find if the book is already in the list
            int found = 0;
            for (int i = 0; i < bookList->count; i++) {
                if (bookList->bookIDs[i] == transaction.bookID) {
                    if (strcmp(transaction.type, "Borrow") == 0) {
                        bookList->borrowedCopies[i] += transaction.copies;
                    } else if (strcmp(transaction.type, "Return") == 0) {
                        bookList->returnedCopies[i] += transaction.copies;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (bookList->count >= MAX_SIZE) {
                    // Reallocate if the current allocation is not enough
                    bookList->bookIDs = realloc(bookList->bookIDs, (bookList->count + MAX_SIZE) * sizeof(int));
                    bookList->borrowedCopies = realloc(bookList->borrowedCopies, (bookList->count + MAX_SIZE) * sizeof(int));
                    bookList->returnedCopies = realloc(bookList->returnedCopies, (bookList->count + MAX_SIZE) * sizeof(int));
                    if (bookList->bookIDs == NULL || bookList->borrowedCopies == NULL || bookList->returnedCopies == NULL) {
                        printf("Memory allocation failed!\n");
                        free(bookList->bookIDs);
                        free(bookList->borrowedCopies);
                        free(bookList->returnedCopies);
                        free(bookList);
                        fclose(fp);
                        return NULL;
                    }
                }
                bookList->bookIDs[bookList->count] = transaction.bookID;
                bookList->borrowedCopies[bookList->count] = (strcmp(transaction.type, "Borrow") == 0) ? transaction.copies : 0;
                bookList->returnedCopies[bookList->count] = (strcmp(transaction.type, "Return") == 0) ? transaction.copies : 0;
                bookList->count++;
            }
        }
    }

    lockFile(fd, F_UNLCK);

    // Unlock the critical section
    pthread_mutex_unlock(&Transactionmutex);

    fclose(fp);
    return bookList;
}


void printBookDetails(BookList *booklist, char str[]) {
    FILE *fp = fopen(booksDB, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    int fd = fileno(fp);  // Get file descriptor associated with the FILE stream

    // Lock the critical section using Bookmutex
    pthread_mutex_lock(&Bookmutex);

    // Acquire read lock
    lockFile(fd, F_RDLCK);

    char line[MAX_SIZE * 4];
    Book book;
    int *foundBooks = calloc(booklist->count, sizeof(int));

    if (foundBooks == NULL) {
        printf("Memory allocation failed!\n");
        fclose(fp);
        return;
    }
    char msg[BUFFER_SIZE];

    fgets(line, sizeof(line), fp);
    strcat(str, "Books borrowed:\n");
    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%d,%[^,],%[^,],%d", &book.id, book.title, book.author, &book.copies);

        for (int i = 0; i < booklist->count; i++) {
            if (book.id == booklist->bookIDs[i] && !foundBooks[i]) {
                int yetToReturn = booklist->borrowedCopies[i] - booklist->returnedCopies[i];
                sprintf(msg, "ID: %d, Title: %s, Author: %s, Borrowed: %d, Returned: %d, Yet to Return: %d\n", 
                        book.id, book.title, book.author, 
                        booklist->borrowedCopies[i], booklist->returnedCopies[i], 
                        yetToReturn);
                strcat(str, msg);
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



void listMembers(Profile *profile,char prompt[]){
    if(profile->admin==1){
        readAndUpdateProfiles(profile->name,profile->password,0,1,prompt);
    }
    else{
        printf("User doesn't have the required permissions.\n");
    }
}


void searchMember(Profile *profile, char Name[], char str[]) {
    BookList *booklist;
    if (profile->admin == 1) {
        printf("User details:-\n");
        strcpy(str, "User details:-\n");
        char temp[BUFFER_SIZE];
        Profile *customer = readAndUpdateProfiles(Name, "", 0, 3, temp);
        booklist = getBookIDsForProfile(customer->id);
    } else if (strcmp(profile->name, Name) == 0) {
        printf("User details sent\n");
        strcpy(str, "User details:-\n");
        booklist = getBookIDsForProfile(profile->id);
    } else {
        printf("This user doesn't have permission to access this data\n");
        return;
    }
    printBookDetails(booklist, str);
}


void transactionList(Profile *profile, char str[]) {
    if (profile->admin == 1) {
        FILE *fp = fopen(transactionsDB, "r");
        if (fp == NULL) {
            printf("Error opening transactionsDB file!\n");
            return;
        }

        int fd = fileno(fp);
        pthread_mutex_lock(&Transactionmutex);
        lockFile(fd, F_RDLCK);

        char line[MAX_SIZE * 5];
        
        strcat(str, "TransactionID,ProfileID,BookID,Copies,Transaction Type\n");
        fgets(line, sizeof(line), fp);

        while (fgets(line, sizeof(line), fp) != NULL) {
            strcat(str, line);
        }

        lockFile(fd, F_UNLCK);
        pthread_mutex_unlock(&Transactionmutex);
        fclose(fp);
    }
}


int deleteMember(char name[], Profile *profile) {
    int found = 0;

    if (profile->admin == 1 || strcmp(profile->name, name) == 0) {
        FILE *fp = fopen(profilesDB, "r");
        if (fp == NULL) {
            printf("Error opening profilesDB file!\n");
            return found;
        }

        int fd = fileno(fp);
        pthread_mutex_lock(&mutex);
        lockFile(fd, F_RDLCK);

        FILE *temp_fp = fopen(TEMP_DB, "w");
        if (temp_fp == NULL) {
            printf("Error opening temporary file!\n");
            fclose(fp);
            pthread_mutex_unlock(&mutex);
            return found;
        }

        char line[MAX_SIZE * 5];
        Profile customer;

        // Read the header line and write it to the temporary file
        if (fgets(line, sizeof(line), fp) != NULL) {
            fputs(line, temp_fp);
        }

        while (fgets(line, sizeof(line), fp) != NULL) {
            sscanf(line, "%d,%[^,],%[^,],%d,%d", &customer.id, customer.name, customer.password, &customer.admin, &customer.borrowed);
            if (strcmp(customer.name, name) != 0) {
                fputs(line, temp_fp);
            } else {
                found = 1;
            }
        }

        fclose(fp);
        fclose(temp_fp);

        if (found) {
            if (remove(profilesDB) != 0) {
                printf("Error removing original profilesDB file!\n");
            } else if (rename(TEMP_DB, profilesDB) != 0) {
                printf("Error renaming temporary file to profilesDB!\n");
            } else {
                printf("Member '%s' deleted successfully.\n", name);
            }
        } else {
            printf("Member '%s' not found in profilesDB.\n", name);
            remove(TEMP_DB);
        }

        pthread_mutex_unlock(&mutex);
    } else {
        printf("The user doesn't have the permission.\n");
    }

    return found;
}