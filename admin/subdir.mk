C_SRCS += \
admin/admin.c

OBJS += \
admin/admin.o

# Each subdirectory must supply rules for building sources it contributes
admin/%.o: admin/%.c
	@echo 'Building file: $^'
	@echo 'Invoking: ARM-GCC C Compiler'
	$(CC) $(CFLAGS) -c -o $@ $^
	@echo 'Finished building: $^'