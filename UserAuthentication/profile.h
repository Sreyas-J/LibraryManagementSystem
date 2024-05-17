#ifndef PROFILE_H
#define PROFILE_H
#include "../supportFunctions/support.h"

void writeProfileToCSV(Profile profile);
Profile *readAndUpdateProfiles(char Name[], int admin, char password[],int copies,int func);
Profile *login(char Name[], int admin, char password[]);
void createProfile(char Name[], int admin,char password[]);

#endif