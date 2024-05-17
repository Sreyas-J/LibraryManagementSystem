#ifndef BOOK_H
#define BOOK_H
#include "../supportFunctions/support.h"

Book *writeTransactionToCSV(Book *book,Profile *profile,int copies,char type[]);
Book *borrowBook(Book *book,Profile *customer,Profile *profile,int copies);
Book *returnBook(Book *book,Profile *customer, Profile *profile,int copies);
Book *updateBookToCSV(char oldTitle[],char oldAuthor[],char newTitle[],char newAuthor[],int newCopies,int func);
Book *addBook(char title[],char author[],Profile *profile,int copies);
Book *modifyBook(char oldTitle[],char oldAuthor[],char newTitle[],char newAuthor[], int newCopies, Profile *profile,int overrride);
void deleteBook(char title[],char author[],Profile *profile);
Book *searchBook(char title[],char author[],Profile *profile);

#endif