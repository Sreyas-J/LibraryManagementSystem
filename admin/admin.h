#ifndef ADMIN_H
#define ADMIN_H
#include "../supportFunctions/support.h"

void updateBookToCSV(char oldTitle[],char oldAuthor[],char newTitle[],char newAuthor[],int newCopies,int func);
void addBook(char title[],char author[],Profile *profile,int copies);
void modifyBook(char oldTitle[],char oldAuthor[],char newTitle[],char newAuthor[], int newCopies, Profile *profile);
void deleteBook(char title[],char author[],Profile *profile);

#endif