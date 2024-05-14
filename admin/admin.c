#include "admin.h"

#include<string.h>
#include<fcntl.h>
#include <stdio.h>
#include<stdlib.h>
#include <pthread.h>

pthread_mutex_t Bookmutex = PTHREAD_MUTEX_INITIALIZER;


void updateBookToCSV(char oldTitle[],char oldAuthor[],char newTitle[],char newAuthor[],int newCopies,int func){
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

        if (strcmp(bookTitle, oldTitle) == 0 && strcmp(bookAuthor, oldAuthor) == 0) {
            // Update the number of oldCopies
            if(func==0) bookCopies += newCopies;

            else if(func==1){
                strcpy(bookTitle,newTitle);
                strcpy(bookAuthor,newAuthor);
                bookCopies=newCopies;
            }

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
    fprintf(fp, "%d,%s,%s,%d\n", nextBookId++, oldTitle, oldAuthor, newCopies);

    // Release the lock
    lockFile(fd, F_UNLCK);

    // Unlock the critical section
    pthread_mutex_unlock(&Bookmutex);

    // Close the file
    fclose(fp);
}


void addBook(char title[],char author[],Profile *profile,int copies){
    if(profile!=NULL && profile->admin==1){
        updateBookToCSV(title,author,title,author,copies,0);
            printf("Successfully %s has added %d oldCopies of %s by %s\n",profile->name,copies,title,author);
    }
    else{
        printf("This user doesn't have the required permisions\n");
    }
}


void modifyBook(char oldTitle[],char oldAuthor[],char newTitle[],char newAuthor[], int newCopies, Profile *profile){
    if(profile!=NULL && profile->admin==1){
        updateBookToCSV(oldTitle,oldAuthor,newTitle,newAuthor,newCopies,1);
        printf("Successfully %s has updated %s by %s to %s by %s of qty. %d\n",profile->name,oldTitle,oldAuthor,newTitle,newAuthor,newCopies);
    }
    else{
        printf("This user doesn't have the required permisions\n");
    }
}


void deleteBook(char title[],char author[],Profile *profile){
    if(profile!=NULL && profile->admin==1){
        updateBookToCSV(title,author,title,author,0,1);
        printf("Successfully %s has deleted %s by %s\n",profile->name,title,author);
    }
    else{
        printf("This user doesn't have the required permisions\n");
    }
}