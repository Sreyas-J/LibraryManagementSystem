C_SRCS += \
socket/server.c

OBJS += \
socket/server.o

# Each subdirectory must supply rules for building sources it contributes
socket/%.o: socket/%.c
	@echo 'Building file: $^'
	@echo 'Invoking: ARM-GCC C Compiler'
	$(CC) $(CFLAGS) -c -o $@ $^
	@echo 'Finished building: $^'