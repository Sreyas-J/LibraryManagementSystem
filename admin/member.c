#include "member.h"
#include "../UserAuthentication/profile.h"
#include "../supportFunctions/support.h"
#include "book.h"

#include<stdio.h>
#include<fcntl.h>

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

    if(type==0){
        book=modifyBook(book->title,book->author,book->title,book->author,book->copies-copies,profile,1);
        profile=login(profile->name,profile->admin,profile->password,copies);
    }

    else{
        book=modifyBook(book->title,book->author,book->title,book->author,book->copies+copies,profile,1);
        profile=login(profile->name,profile->admin,profile->password,-copies);
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