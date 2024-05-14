#include "profile.h"
#include "../supportFunctions/support.h"

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 


int nextProfileId = 1;  


void writeProfileToCSV(Profile profile) {
    int fd;
    FILE *fp;
    struct flock lock;

    // Open the file in append mode
    fp = fopen(profilesDB, "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Get the file descriptor associated with the FILE stream
    fd = fileno(fp);

    // Acquire the lock
    lockFile(fd, F_WRLCK); // Exclusive write lock

    // Assign ID and write data to the file
    profile.id = nextProfileId++;
    fprintf(fp, "%d,%s,%s,%d,%d\n", profile.id, profile.name,profile.password, profile.admin, profile.borrowed);

    // Release the lock
    lockFile(fd, F_UNLCK);

    // Close the file
    fclose(fp);
}


Profile *login(char Name[], int admin, char password[]) {
    FILE *fp;
    int fd;
    fp = fopen(profilesDB, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    fd = fileno(fp);  // Get file descriptor associated with the FILE stream
    lockFile(fd, F_RDLCK);  // Acquire read lock

    char line[MAX_SIZE * 10];

    while (fgets(line, sizeof(line), fp) != NULL) {
        int profileId, profileBorrowed, profileAdmin;
        char profileName[MAX_SIZE], profilePassword[MAX_SIZE];

        sscanf(line, "%d,%[^,],%[^,],%d,%d", &profileId, profileName, profilePassword, &profileAdmin, &profileBorrowed);

        if (strcmp(profileName, Name) == 0 && strcmp(profilePassword, password) == 0 && profileAdmin == admin) {
            Profile *foundProfile = malloc(sizeof(Profile));  // Allocate memory for foundProfile
            if (foundProfile == NULL) {
                printf("Memory allocation failed!\n");
                fclose(fp);
                return NULL;
            }

            foundProfile->id = profileId;
            strcpy(foundProfile->name, profileName);
            foundProfile->borrowed = profileBorrowed;
            foundProfile->admin = profileAdmin;

            lockFile(fd, F_UNLCK);  // Release the read lock
            fclose(fp);
            return foundProfile;
        }
    }

    lockFile(fd, F_UNLCK);  // Release the read lock
    fclose(fp);
    printf("No such account exists\n");
    return NULL;
}


void createProfile(char Name[], int admin,char password[]) {
    Profile profile;
    strcpy(profile.password,password);
    profile.borrowed = 0;
    profile.admin = admin;
    strcpy(profile.name, Name);

    // Write profile data to CSV file
    writeProfileToCSV(profile);
}


int main() {
    FILE *fp = fopen(profilesDB, "w");
    fprintf(fp, "");
    fclose(fp);

    createProfile("Sreyas", 1, "password");
    createProfile("J", 1, "p");

    createProfile("Some", 0, "a");
    createProfile("None", 0, "s");

    Profile *profile1 = login("Sreyas", 1, "password");
    Profile *profile2 = login("Sreyas", 0, "password");

    if (profile1 != NULL) {
        printf("Profile found - ID: %d, Name: %s\n", profile1->id, profile1->name);
        free(profile1);  // Free allocated memory when done
    }

    if (profile2 != NULL) {
        printf("Profile found - ID: %d, Name: %s\n", profile2->id, profile2->name);
        free(profile2);  // Free allocated memory when done
    }

    return 0;
}
