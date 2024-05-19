#include "profile.h"
#include "../admin/book.h"
#include "../admin/member.h"
#include "../supportFunctions/support.h"
#include "../socket/server.h"

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void writeProfileToCSV(Profile profile) {
    int fd;
    FILE *fp;

    // Open the file in append mode
    fp = fopen(profilesDB, "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Get the file descriptor associated with the FILE stream
    fd = fileno(fp);

    // Lock the critical section using mutex
    pthread_mutex_lock(&mutex);
    lockFile(fd, F_WRLCK); // Exclusive write lock

    // Assign ID and write data to the file
    profile.id = nextProfileId++;
    fprintf(fp, "%d,%s,%s,%d,%d\n", profile.id, profile.name, profile.password, profile.admin, profile.borrowed);

    // Unlock the critical section
    lockFile(fd, F_UNLCK);
    pthread_mutex_unlock(&mutex);

    // Close the file
    fclose(fp);
}


Profile *readAndUpdateProfiles(char Name[], char password[],int copies,int func,char prompt[]) {
    FILE *fp;
    int fd;
    fp = fopen(profilesDB, "r+");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    fd = fileno(fp);  // Get file descriptor associated with the FILE stream

    // Lock the critical section using mutex
    pthread_mutex_lock(&mutex);

    // Acquire read lock
    if(func!=2) lockFile(fd, F_RDLCK);
    else lockFile(fd,F_WRLCK);

    char line[MAX_SIZE * 10];
    Profile *foundProfile = NULL;  // Initialize to NULL

    long int pos = 0;

    char temp[BUFFER_SIZE];
    if(func==1){ 
        sprintf(prompt,"\nMEMBERS DETAILS:-\nProfileID      Name                  No.of Books Borrowed\n");
    }

    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp) != NULL) {
        int profileId, profileBorrowed, profileAdmin;
        char profileName[MAX_SIZE], profilePassword[MAX_SIZE];


        sscanf(line, "%d,%[^,],%[^,],%d,%d", &profileId, profileName, profilePassword, &profileAdmin, &profileBorrowed);
        if(func==1 && profileAdmin==0){
            sprintf(temp,"%d              %s                     %d\n",profileId,profileName,profileBorrowed);
            strcat(prompt,temp);
        }

        if ((strcmp(profileName, Name) == 0) && ((strcmp(profilePassword,password)==0 && (func==0||func==2))||(func==3))) {
            foundProfile = malloc(sizeof(Profile));
            if (foundProfile == NULL) {
                printf("Memory allocation failed!\n");
                break;  // Exit loop if memory allocation fails
            }

            foundProfile->id = profileId;
            strcpy(foundProfile->name, profileName);
            strcpy(foundProfile->password,profilePassword);
            foundProfile->borrowed = profileBorrowed;
            foundProfile->admin = profileAdmin;
            if(func==0){
                  // Allocate memory for foundProfile

                printf("Profile found %s %d.\n",profileName,profileAdmin);
            }
            else if(func==2){
                pos = ftell(fp) - strlen(line);
                foundProfile->borrowed+=copies;
                // Rewind the file to the position of the found entry
                fseek(fp, pos, SEEK_SET);
                fprintf(fp, "%d,%s,%s,%d,%d\n", foundProfile->id, foundProfile->name, foundProfile->password, foundProfile->admin, foundProfile->borrowed);
            }

            if(func!=1) break;  // Exit loop since profile is found
        }
    }

    // Release the read lock
    lockFile(fd, F_UNLCK);

    // Unlock the critical section
    pthread_mutex_unlock(&mutex);

    // Close the file
    fclose(fp);

    if(func==1) return NULL;

    if (foundProfile == NULL) {
        printf("No such account exists\n");
    }

    return foundProfile;
}


Profile *login(char Name[], char password[]){
    char *temp;
    return readAndUpdateProfiles(Name,password,0,0,temp);
}


void createProfile(char Name[], int admin,char password[]) {
    Profile profile;
    strcpy(profile.password,password);
    profile.borrowed = 0;
    profile.admin = admin;
    strcpy(profile.name, Name);

    // Write profile data to CSV file
    printf("Succesfully created the account\n");
    writeProfileToCSV(profile);
}