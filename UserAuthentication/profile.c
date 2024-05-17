#include "profile.h"
#include "../admin/book.h"
#include "../admin/member.h"

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


Profile *login(char Name[], int admin, char password[],int copies) {
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
    if(copies<0) lockFile(fd, F_RDLCK);
    else lockFile(fd,F_WRLCK);

    char line[MAX_SIZE * 10];
    Profile *foundProfile = NULL;  // Initialize to NULL

    long int pos = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        int profileId, profileBorrowed, profileAdmin;
        char profileName[MAX_SIZE], profilePassword[MAX_SIZE];


        sscanf(line, "%d,%[^,],%[^,],%d,%d", &profileId, profileName, profilePassword, &profileAdmin, &profileBorrowed);

        if (strcmp(profileName, Name) == 0 && strcmp(profilePassword, password) == 0 && profileAdmin == admin) {
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
            if(copies==0){
                  // Allocate memory for foundProfile

                printf("Profile found %s %d.\n",profileName,profileAdmin);
            }
            else{
                pos = ftell(fp) - strlen(line);
                foundProfile->borrowed+=copies;
                // Rewind the file to the position of the found entry
                fseek(fp, pos, SEEK_SET);
                fprintf(fp, "%d,%s,%s,%d,%d\n", foundProfile->id, foundProfile->name, foundProfile->password, foundProfile->admin, foundProfile->borrowed);
            }

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

    FILE *f1 = fopen(transactionsDB, "w");
    fprintf(f1, "\n");  // Write a newline character to the file
    fclose(f1);

    createProfile("Sreyas", 1, "password");
    createProfile("J", 1, "p");

    createProfile("Some", 0, "a");
    createProfile("None", 0, "s");

    Profile *profile1 = login("Sreyas", 1, "password",0);
    Profile *profile2 = login("Some", 0, "a",0);

    addBook("title","author",profile1,2);
    addBook("title","author",profile2,2);
    addBook("title","author",profile1,2);
    addBook("ti","au",profile1,3);

    deleteBook("title","author",profile1);
    deleteBook("ti","au",profile2);

    modifyBook("ti","au","it","ut",5,profile1,0);
    modifyBook("ti","au","it","ut",4,profile2,0);

    Book *book1=searchBook("it","",profile1);
    Book *book2=searchBook("","ut",profile1);
    Book *book3=searchBook("title","",profile1);

    book1=borrowBook(book1,profile2,profile1,3);
    book1=borrowBook(book1,profile1,profile1,3);

    book1=returnBook(book1,profile2,profile1,3);

    if (profile1 != NULL) {
        free(profile1);  // Free allocated memory when done
    }

    if (profile2 != NULL) {
        free(profile2);  // Free allocated memory when done
    }

    free(book1);
    free(book2);
    free(book3);
    
    return 0;
}
