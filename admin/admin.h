#ifndef ADMIN_H
#define ADMIN_H
#include "../supportFunctions/support.h"

Book *readBookFromCSV(char title[], char author[]);
void updateBookToCSV(char title[],char author[],int copies);
void addBook(char title[],char author[],Profile *profile,int copies);

#endif