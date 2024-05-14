C_SRCS += \
supportFunctions/support.c

OBJS += \
supportFunctions/support.o

# Each subdirectory must supply rules for building sources it contributes
supportFunctions/%.o: supportFunctions/%.c
	@echo 'Building file: $^'
	@echo 'Invoking: ARM-GCC C Compiler'
	$(CC) $(CFLAGS) -c -o $@ $^
	@echo 'Finished building: $^'