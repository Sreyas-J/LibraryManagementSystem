#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 

#define MAX_SIZE 20


char profilesDB[] = "../profiles.csv";
int nextProfileId = 1;  


struct Book {
    int id;
    char title[MAX_SIZE];
    char author[MAX_SIZE];
} typedef Book;

struct Profile {
    int id;
    char name[MAX_SIZE];
    int books[MAX_SIZE];
    char password[MAX_SIZE];
    int borrowed;
    int admin;
} typedef Profile;


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

    // Set up the lock structure for advisory locking
    lock.l_type = F_WRLCK; // Exclusive write lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    // Acquire the lock
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error acquiring lock");
        fclose(fp);
        return;
    }

    // Assign ID and write data to the file
    profile.id = nextProfileId++;
    fprintf(fp, "%d,%s,%s,%d,%d\n", profile.id, profile.name,profile.password, profile.admin, profile.borrowed);

    // Release the lock
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error releasing lock");
    }

    // Close the file
    fclose(fp);
}


Profile *login(char Name[], int admin, char password[]) {
    FILE *fp;
    fp = fopen(profilesDB, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    char line[MAX_SIZE * 10];
    fgets(line, sizeof(line), fp);  // Read and ignore the header line

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

            fclose(fp);
            return foundProfile;
        }
    }

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
