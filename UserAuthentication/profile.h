#ifndef PROFILE_H
#define PROFILE_H
#include "../supportFunctions/support.h"

void writeProfileToCSV(Profile profile);
Profile *readAndUpdateProfiles(char Name[], char password[],int copies,int func,char prompt[]);
Profile *login(char Name[], char password[]);
void createProfile(char Name[], int admin,char password[]);

#endif