CC = gcc
CFLAGS = -Wall -Wextra -g

C_SRCS += \
    UserAuthentication/profile.c \
    supportFunctions/support.c \
	admin/admin.c

OBJS = $(C_SRCS:.c=.o)

DBS += \
	profiles.csv \
	books.csv

# Rule for building object files
%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: $(CC) C Compiler'
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo 'Finished building: $<'

$(DBS):
	touch $(DBS)

# Rule for building the executable
lms: $(OBJS) $(DBS)
	@echo 'Building executable: $@'
	@echo 'Invoking: $(CC) Linker'
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	@echo 'Finished building executable: $@'

# Clean rule
clean:
	@echo 'Cleaning up...'
	rm -f $(OBJS) lms
	@echo 'Cleanup complete.'

# Phony target to prevent conflicts with files named 'clean' or 'all'
.PHONY: clean all
