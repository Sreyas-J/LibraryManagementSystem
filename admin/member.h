#ifndef MEMBER_H
#define MEMBER_H

#include "../supportFunctions/support.h"

Book *writeTransactionToCSV(Book *book,Profile *profile,int copies,char type[]);
Book *borrowBook(Book *book,Profile *customer,Profile *profile,int copies);
Book *returnBook(Book *book,Profile *customer, Profile *profile,int copies);

#endif