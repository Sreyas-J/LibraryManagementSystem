CC = gcc
CFLAGS = -Wall -Wextra

C_SRCS += \
    UserAuthentication/profile.c \
    supportFunctions/support.c

OBJS = $(C_SRCS:.c=.o)

# Rule for building object files
%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: $(CC) C Compiler'
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo 'Finished building: $<'

# Rule for building the executable
lms: $(OBJS)
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