#ifndef MEMBER_H
#define MEMBER_H

#include "../UserAuthentication/profile.h"

void listMembers(Profile *profile,char prompt[]);
void searchMember(Profile *profile,char Name[],char str[]);

#endif