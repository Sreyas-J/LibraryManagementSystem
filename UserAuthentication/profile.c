#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_SIZE 20

// extern users_size;
// extern Profile users[];
// extern Profile admins[];
// extern admins_size;

struct Book {
    char title[MAX_SIZE];
    char author[MAX_SIZE];
} typedef Book;

struct Profile {
    char name[MAX_SIZE];
    Book books[MAX_SIZE];
    char password[MAX_SIZE];
    int borrowed;
    int admin;
} typedef Profile;

Profile users[MAX_SIZE];
int users_size = 0;
Profile admins[MAX_SIZE];
int admins_size = 0;
char profilesDB[]="../profiles.csv";

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

    // Write data to the file
    fprintf(fp, "%s,%d,%d\n", profile.name, profile.borrowed, profile.admin);

    // Release the lock
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error releasing lock");
    }

    // Close the file
    fclose(fp);
}

void createProfile(char Name[], int admin,char password[]) {
    Profile profile;
    strcpy(profile.password,password);
    profile.borrowed = 0;
    profile.admin = admin;
    strcpy(profile.name, Name);

    if (profile.admin) {
        admins[admins_size++] = profile;
    } else {
        users[users_size++] = profile;
    }

    // Write profile data to CSV file
    writeProfileToCSV(profile);
}

void login()

int main() {
    createProfile("Sreyas", 1);
    createProfile("J", 1);

    createProfile("Some", 0);
    createProfile("None", 0);

    return 0;
}
