
# Library Management System

In the contemporary digital age, efficient management systems are crucial for the smooth functioning of
libraries. This mini project aims to design and develop an Library Management System (LMS)
that not only provides basic functionalities of a library but also ensures data security and concurrency
control. The system employs socket programming to enable multiple clients to access the library
database concurrently. Additionally, system calls are utilized extensively for various operations such as
process management, file handling, file locking, multithreading, and interprocess communication,
ensuring optimal performance and resource utilization.

The proliferation of digital technologies has revolutionized the way libraries operate, necessitating the
adoption of robust management systems to streamline processes and enhance user experience. TheLibrary Management System (LMS) proposed in this project is designed to address these needs by offering a interface coupled with advanced functionalities.
## IMPLEMENTATION

**Multi-threading** is implemented so that server can allow multiple clients at once.

The **IPC**(Inter-process communication) between server and client is facilitated using sockets.

**Makefile** has been extensively used to compile and create 2 executables: server,client.

The **database** is managed in multiple csv files and **concurrency** is acheived using file locks and mutexes.
Each csv has its own file lock and mutex.



## Extra Features

- A member can't borrow more than 3 books at once.
- An admin must enter the admin username, password when a customer tries to borrow or return books. This is to ensure the transaction is valid.
- An admin account is pre-created with username:admin, password:admin and the program only allows to create member accounts.


## CODE DOCUMENTATION

**supportFunctions/**

This folder contains support.c,support.h,subdir.mk.

- support.h: Global variables,mutexes declaration and structures definition. Also include function declaration for used for file (un)locking.
- support.c: Initialised global variables values,mutexes and provides the lockfile function definition.
- subdir.mk: A makefile compiles all .c files in the folder into .o files.

**admin/**

This folder contains book.c, book.h, member.c, member.h, subdir.mk.
- book.h: Lists all the function declarations dealing with CRUD functionalities on books.
- book.c: It has function definition of all the declarations in book.h.
- member.h: Lists all the function declarations dealing with CRUD functionality on profiles.
- member.c: It has the function definition of all the declarations in member.h.

**UserAuthentication/**

This folder contains profile.c,profile.h,subdir.mk

- profile.h: Contains function declaration of signup,login and its helper functions.
- profile.c: It has the function definition of the declarations in profile.h

**socket/**

This folder contains server.c,server.h,client.c,client.h,subdir.mk

- server.c: Contains the main function for server side, it uses all the above c code. This is where multithreading and sockets is implemented. This function creates 3 empty csv only with the headers: profiles.csv, books.csv, transaction.csv.

- client.c: Contains the main function for client side it uses sockets to communicate with the server.

**makefile**

Provides the following functionalities:
- make client : Creates the client executable file.
- make server : Creates the server executable file.
- make clean : Clears the executables, csvs and .o files.

## Run Locally

In the root directory of the project run the following commands in the terminal:-
- make clean
- make server
- make client
- ./server

Open another terminal and run:

- ./client

Enter "admin" as username and "admin" as password.

You have successfully logged in with admin account now enter any command which is available in the admin menu displayed on ur screen.

Now create more client instances and try creating members and play with their menu options.

