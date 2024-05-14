C_SRCS += \
UserAuthentication/profile.c

OBJS += \
UserAuthentication/profile.o

# Each subdirectory must supply rules for building sources it contributes
UserAuthentication/%.o: UserAuthentication/%.c
	@echo 'Building file: $^'
	@echo 'Invoking: ARM-GCC C Compiler'
	$(CC) $(CFLAGS) -c -o $@ $^
	@echo 'Finished building: $^'