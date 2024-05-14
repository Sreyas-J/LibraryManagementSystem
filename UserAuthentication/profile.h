#ifndef PROFILE_H
#define PROFILE_H
#include "../supportFunctions/support.h"

pthread_mutex_t mutex;
void writeProfileToCSV(Profile profile);
Profile *login(char Name[], int admin, char password[]);
void createProfile(char Name[], int admin,char password[]);

#endif