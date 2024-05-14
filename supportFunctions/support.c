#include "support.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h> 

void lockFile(int fd, int lockType) {
    struct flock lock;
    lock.l_type = lockType;  // F_RDLCK for read lock, F_WRLCK for write lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error acquiring lock");
        exit(EXIT_FAILURE);
    }
}