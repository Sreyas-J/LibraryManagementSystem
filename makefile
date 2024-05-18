CC = gcc
CFLAGS = -Wall -Wextra -g

# Source files
C_SRCS = \
    UserAuthentication/profile.c \
    supportFunctions/support.c \
    admin/book.c \
    admin/member.c

CLIENT_SRCS = \
    socket/client.c

S_SRCS = \
    socket/server.c

# Object files
C_OBJS = $(C_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
S_OBJS = $(S_SRCS:.c=.o)

# Database files
DBS = \
    profiles.csv \
    books.csv \
    transaction.csv

# Rule for building object files
%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: $(CC) C Compiler'
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo 'Finished building: $<'

# Rule for creating database files
$(DBS):
	touch $(DBS)

# Rule for building the LMS executable
lms: $(C_OBJS) $(DBS)
	@echo 'Building executable: $@'
	@echo 'Invoking: $(CC) Linker'
	$(CC) $(CFLAGS) -o $@ $(C_OBJS)
	@echo 'Finished building executable: $@'

# Rule for building the server executable
server: $(S_OBJS) $(C_OBJS)
	@echo 'Building executable: $@'
	@echo 'Invoking: $(CC) Linker'
	$(CC) $(CFLAGS) -o $@ $(S_OBJS) $(C_OBJS)
	@echo 'Finished building executable: $@'

# Rule for building the client executable
client: $(CLIENT_OBJS)
	@echo 'Building executable: $@'
	@echo 'Invoking: $(CC) Linker'
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJS)
	@echo 'Finished building executable: $@'

# Clean rule
clean:
	@echo 'Cleaning up...'
	rm -f $(C_OBJS) $(S_OBJS) $(CLIENT_OBJS) server client lms
	@echo 'Cleanup complete.'

# Phony target to prevent conflicts with files named 'clean' or 'all'
.PHONY: clean all
