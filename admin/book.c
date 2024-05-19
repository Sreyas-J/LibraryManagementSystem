#include "book.h"
#include "member.h"
#include "../UserAuthentication/profile.h"
#include "../supportFunctions/support.h"
#include "../socket/server.h"

#include<string.h>
#include<fcntl.h>
#include <stdio.h>
#include<stdlib.h>
#include <pthread.h>

pthread_mutex_t Bookmutex = PTHREAD_MUTEX_INITIALIZER;


void printBooks(char str[]) {
    FILE *file = fopen(booksDB, "r");
    if (file == NULL) {
        perror("Could not open file");
        return;
    }

    char line[MAX_SIZE];
    // Skip the header line
    if (fgets(line, sizeof(line), file) == NULL) {
        perror("Could not read header");
        fclose(file);
        return;
    }

    strcpy(str,"Books:-\n");
    char temp[BUFFER_SIZE];
    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file)) {
        Book book;
        sscanf(line, "%d,%[^,],%[^,],%d", &book.id, book.title, book.author, &book.copies);

        // Print the book details
        sprintf(temp,"ID: %d, Title: %s, Author: %s, Copies: %d\n", book.id, book.title, book.author, book.copies);
        strcat(str,temp);
    }

    fclose(file);
}


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


Book *addBook(char title[],char author[],Profile *profile,int copies){
    if(profile!=NULL && profile->admin==1){
        Book *book=updateBookToCSV(title,author,title,author,copies,0);
        printf("Successfully %s has added %d copies of %s by %s\n",profile->name,copies,title,author);
        return book;
    }
    else{
        printf("This user doesn't have the required permisions\n");
    }
    return NULL;
}


Book *modifyBook(char oldTitle[],char oldAuthor[],char newTitle[],char newAuthor[], int newCopies, Profile *profile,int override){
    if(profile!=NULL && (profile->admin==1 || override==1)){
        Book *book=updateBookToCSV(oldTitle,oldAuthor,newTitle,newAuthor,newCopies,1);
        printf("Successfully %s has updated %s by %s to %s by %s of qty. %d\n",profile->name,oldTitle,oldAuthor,newTitle,newAuthor,newCopies);
        return book;
    }
    else{
        printf("This user doesn't have the required permisions\n");
    }
    return NULL;
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
    if(profile!=NULL){
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


Book *writeTransactionToCSV(Book *book,Profile *profile,int copies,char type[]){
    int fd;
    FILE *fp;

    // Open the file in append mode
    fp = fopen(transactionsDB, "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    // Get the file descriptor associated with the FILE stream
    fd = fileno(fp);

    pthread_mutex_lock(&mutex);
    lockFile(fd, F_WRLCK); 

    fprintf(fp, "%d,%d,%d,%d,%s\n", nextTransactionId++, profile->id, book->id, copies,type);

    // Unlock the critical section
    lockFile(fd, F_UNLCK);
    pthread_mutex_unlock(&mutex);

    // Close the file
    fclose(fp);

    if(strcmp(type,"Borrow")==0){
        book=modifyBook(book->title,book->author,book->title,book->author,book->copies-copies,profile,1);
        profile=readAndUpdateProfiles(profile->name,profile->password,copies,2);
    }

    else if(strcmp(type,"Return")==0){
        book=modifyBook(book->title,book->author,book->title,book->author,book->copies+copies,profile,1);
        profile=readAndUpdateProfiles(profile->name,profile->password,-copies,2);
    }

    return book;
}


Book *borrowBook(Book *book,Profile *customer,Profile *profile,int copies){
    if(profile->admin==1){
        if(customer->borrowed+copies<=3){
            if(book->copies>=copies){
                
                book=writeTransactionToCSV(book,customer,copies,"Borrow");

                printf("%s has successfully borrowed %d copies of %s by %s.\n",customer->name,copies,book->title,book->author);
            }
            else{
                printf("The library has only %d copies of %s by %s. So, can't lend %d copies.\n",book->copies,book->title,book->author,copies);
            }
            
        }
        else{
            printf("This user has already borrowed %d books and the limit on borrowed books at any moment is 3-> So can't borrow %d books.\n",customer->borrowed,copies);
        }
    }
    else{
        printf("You can only borrow books in the presence of an admin. So, can't borrow now.\n");
    }
    return book;
}


Book *returnBook(Book *book,Profile *customer, Profile *profile,int copies){
    if(profile->admin==1){
        book=writeTransactionToCSV(book,customer,copies,"Return");
        printf("%s has successfully returned %d copies of %s by %s.\n",customer->name,copies,book->title,book->author);
    }
    else{
        printf("You can only return books in the presence of an admin. So, can't return now.\n");
    }
    return book;
}