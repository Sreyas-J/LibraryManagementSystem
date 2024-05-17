#include "member.h"
#include "../supportFunctions/support.h"
#include "../UserAuthentication/profile.h"
#include<stdio.h>

void listMembers(Profile *profile){
    if(profile->admin==1){
        readAndUpdateProfiles(profile->name,profile->admin,profile->password,0,1);
    }
    else{
        printf("User doesn't have the required permissions.\n");
    }
}