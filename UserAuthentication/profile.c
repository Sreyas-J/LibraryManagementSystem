#include "profile.h"
#include "../admin/admin.h"

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 


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


Profile *login(char Name[], int admin, char password[]) {
    FILE *fp;
    int fd;
    fp = fopen(profilesDB, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    fd = fileno(fp);  // Get file descriptor associated with the FILE stream

    // Lock the critical section using mutex
    pthread_mutex_lock(&mutex);

    // Acquire read lock
    lockFile(fd, F_RDLCK);

    char line[MAX_SIZE * 10];
    Profile *foundProfile = NULL;  // Initialize to NULL

    while (fgets(line, sizeof(line), fp) != NULL) {
        int profileId, profileBorrowed, profileAdmin;
        char profileName[MAX_SIZE], profilePassword[MAX_SIZE];

        sscanf(line, "%d,%[^,],%[^,],%d,%d", &profileId, profileName, profilePassword, &profileAdmin, &profileBorrowed);

        if (strcmp(profileName, Name) == 0 && strcmp(profilePassword, password) == 0 && profileAdmin == admin) {
            foundProfile = malloc(sizeof(Profile));  // Allocate memory for foundProfile
            if (foundProfile == NULL) {
                printf("Memory allocation failed!\n");
                break;  // Exit loop if memory allocation fails
            }

            foundProfile->id = profileId;
            strcpy(foundProfile->name, profileName);
            strcpy(foundProfile->password,profilePassword);
            foundProfile->borrowed = profileBorrowed;
            foundProfile->admin = profileAdmin;

            printf("Profile found %s %d\n",profileName,profileAdmin);

            break;  // Exit loop since profile is found
        }
    }

    // Release the read lock
    lockFile(fd, F_UNLCK);

    // Unlock the critical section
    pthread_mutex_unlock(&mutex);

    // Close the file
    fclose(fp);

    if (foundProfile == NULL) {
        printf("No such account exists\n");
    }

    return foundProfile;
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


int main() {
    FILE *fp = fopen(profilesDB, "w");
    fprintf(fp, "\n");  // Write a newline character to the file
    fclose(fp);

    FILE *f = fopen(booksDB, "w");
    fprintf(f, "\n");  // Write a newline character to the file
    fclose(f);

    createProfile("Sreyas", 1, "password");
    createProfile("J", 1, "p");

    createProfile("Some", 0, "a");
    createProfile("None", 0, "s");

    Profile *profile1 = login("Sreyas", 1, "password");
    Profile *profile2 = login("Some", 0, "a");

    addBook("title","author",profile1,2);
    addBook("title","author",profile2,2);
    addBook("title","author",profile1,2);
    addBook("ti","au",profile1,3);

    deleteBook("title","author",profile1);
    deleteBook("ti","au",profile2);

    modifyBook("ti","au","it","ut",5,profile1);
    modifyBook("ti","au","it","ut",4,profile2);

    searchBook("it","",profile1);
    searchBook("","ut",profile1);
    searchBook("title","",profile1);


    if (profile1 != NULL) {
        free(profile1);  // Free allocated memory when done
    }

    if (profile2 != NULL) {
        free(profile2);  // Free allocated memory when done
    }

    return 0;
}
