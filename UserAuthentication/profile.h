// #ifndef PROFILE_H
// #define PROFILE_H

#define MAX_SIZE 20

char profilesDB[] = "../profiles.csv";

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

// #endif PROFILE_H