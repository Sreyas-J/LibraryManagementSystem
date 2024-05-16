#include "admin.h"

#include<string.h>
#include<fcntl.h>
#include <stdio.h>
#include<stdlib.h>
#include <pthread.h>

pthread_mutex_t Bookmutex = PTHREAD_MUTEX_INITIALIZER;


Book *updateBookToCSV(char oldTitle[], char oldAuthor[], char newTitle[], char newAuthor[], int newCopies, int func) {
    FILE *fp;
    int fd;
    Book *book = malloc(sizeof(Book));
    if (book == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    fp = fopen(booksDB, "r+");
    if (fp == NULL) {
        printf("Error opening file!\n");
        free(book);
        return NULL;
    }

    fd = fileno(fp);  // Get file descriptor associated with the FILE stream

    // Lock the critical section using Bookmutex
    pthread_mutex_lock(&Bookmutex);

    // Acquire write lock
    lockFile(fd, F_WRLCK);

    char line[MAX_SIZE * 10];
    long int pos = 0;  // To store the position of the found book entry

    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%d,%[^,],%[^,],%d", &book->id, book->title, book->author, &book->copies);

        if (func != 2) {
            if (strcmp(book->title, oldTitle) == 0 && strcmp(book->author, oldAuthor) == 0) {
                if (func == 0) {
                    book->copies += newCopies;  // Update the number of copies
                } else if (func == 1) {
                    strcpy(book->title, newTitle);
                    strcpy(book->author, newAuthor);
                    book->copies = newCopies;
                }

                // Store the position of the found entry
                pos = ftell(fp) - strlen(line);

                // Rewind the file to the position of the found entry
                fseek(fp, pos, SEEK_SET);

                // Write the updated data back to the file
                fprintf(fp, "%d,%s,%s,%d\n", book->id, book->title, book->author, book->copies);

                // Release the lock
                lockFile(fd, F_UNLCK);

                // Unlock the critical section
                pthread_mutex_unlock(&Bookmutex);

                // Close the file
                fclose(fp);
                return book;
            }
        } else if (func == 2) {
            if (strlen(oldTitle) > 1 && book->copies>0 && strcmp(book->title, oldTitle) == 0) {
                // Release the lock
                lockFile(fd, F_UNLCK);

                // Unlock the critical section
                pthread_mutex_unlock(&Bookmutex);

                // Close the file
                fclose(fp);
                return book;
            }
            if (strlen(oldAuthor) > 1 && book->copies>0 && strcmp(book->author, oldAuthor) == 0) {
                // Release the lock
                lockFile(fd, F_UNLCK);

                // Unlock the critical section
                pthread_mutex_unlock(&Bookmutex);

                // Close the file
                fclose(fp);
                return book;
            }
        }
    }

    // If the book does not exist and func is not 2, append it to the file
    if (func != 2) {
        book->id = nextBookId++;
        strcpy(book->title, oldTitle);
        strcpy(book->author, oldAuthor);
        book->copies = newCopies;

        fseek(fp, 0, SEEK_END);  // Move to the end of the file
        fprintf(fp, "%d,%s,%s,%d\n", book->id, book->title, book->author, book->copies);
    } else {
        free(book);
        book = NULL;
    }

    // Release the lock
    lockFile(fd, F_UNLCK);

    // Unlock the critical section
    pthread_mutex_unlock(&Bookmutex);

    // Close the file
    fclose(fp);
    return book;
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


Book *searchBook(char title[],char author[],Profile *profile){
    if(profile!=NULL && profile->admin==1){
        Book *book;
        if(strlen(title)>1){
            book=updateBookToCSV(title,author,title,author,0,2);
        }
        else if(strlen(author)>1){
            book=updateBookToCSV(title,author,title,author,0,2);
        }

        if(book!=NULL){
                printf("Found the book %s by %s with qty. %d\n",book->title,book->author,book->copies);
            }
            else{
                printf("No such book found...\n");
            }
            return book;
        }
    else{
        printf("This user doesn't have the required permisions\n");
    }
    return NULL;
}