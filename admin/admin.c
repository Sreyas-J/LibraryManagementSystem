#include "admin.h"

#include<string.h>
#include<fcntl.h>
#include <stdio.h>
#include<stdlib.h>
#include <pthread.h>

pthread_mutex_t Bookmutex = PTHREAD_MUTEX_INITIALIZER;


Book *readBookFromCSV(char title[], char author[]) {
    FILE *fp;
    int fd;
    fp = fopen(booksDB, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    fd = fileno(fp);  // Get file descriptor associated with the FILE stream
    lockFile(fd, F_RDLCK);  // Acquire read lock

    char line[MAX_SIZE * 10];

    while (fgets(line, sizeof(line), fp) != NULL) {
        int bookId, bookCopies;
        char bookTile[MAX_SIZE], bookAuthor[MAX_SIZE];

        sscanf(line, "%d,%[^,],%[^,],%d", &bookId, bookTile, bookAuthor, &bookCopies);

        if (strcmp(bookTile, title) == 0 && strcmp(bookAuthor, author) == 0) {
            Book *foundBook = malloc(sizeof(Book));  // Allocate memory for foundProfile
            if (foundBook == NULL) {
                printf("Memory allocation failed!\n");
                fclose(fp);
                return NULL;
            }

            foundBook->id = bookId;
            strcpy(foundBook->title,title);
            strcpy(foundBook->author,author);
            foundBook->copies = bookCopies;

            lockFile(fd, F_UNLCK);  // Release the read lock
            fclose(fp);
            return foundBook;
        }
    }

    lockFile(fd, F_UNLCK);  // Release the read lock
    fclose(fp);
    printf("No such book exists\n");
    return NULL;
}


void updateBookToCSV(char title[], char author[], int copies) {
    FILE *fp;
    int fd;
    fp = fopen(booksDB, "r+");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fd = fileno(fp);  // Get file descriptor associated with the FILE stream

    // Lock the critical section using Bookmutex
    pthread_mutex_lock(&Bookmutex);

    // Acquire write lock
    lockFile(fd, F_WRLCK);

    char line[MAX_SIZE * 10];
    long int pos = 0;  // To store the position of the found book entry

    while (fgets(line, sizeof(line), fp) != NULL) {
        int bookId, bookCopies;
        char bookTitle[MAX_SIZE], bookAuthor[MAX_SIZE];

        sscanf(line, "%d,%[^,],%[^,],%d", &bookId, bookTitle, bookAuthor, &bookCopies);

        if (strcmp(bookTitle, title) == 0 && strcmp(bookAuthor, author) == 0) {
            // Update the number of copies
            bookCopies += copies;

            // Store the position of the found entry
            pos = ftell(fp) - strlen(line);

            // Rewind the file to the position of the found entry
            fseek(fp, pos, SEEK_SET);

            // Write the updated data back to the file
            fprintf(fp, "%d,%s,%s,%d\n", bookId, bookTitle, bookAuthor, bookCopies);

            // Release the lock
            lockFile(fd, F_UNLCK);

            // Unlock the critical section
            pthread_mutex_unlock(&Bookmutex);

            // Close the file
            fclose(fp);
            return;
        }
    }

    // If the book does not exist, simply append it to the file
    fseek(fp, 0, SEEK_END);  // Move to the end of the file
    fprintf(fp, "%d,%s,%s,%d\n", nextBookId++, title, author, copies);

    // Release the lock
    lockFile(fd, F_UNLCK);

    // Unlock the critical section
    pthread_mutex_unlock(&Bookmutex);

    // Close the file
    fclose(fp);
}


void addBook(char title[],char author[],Profile *profile,int copies){
    if(profile!=NULL){
        if(profile->admin==1){
            updateBookToCSV(title,author,copies);
            printf("Successfully %s has added %d copies of %s by %s\n",profile->name,copies,title,author);
        }
        else{
            printf("This user doesn't have the required permisions\n");
        }
    }
}