################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freertos-code/FreeRTOS/Source/croutine.c \
../freertos-code/FreeRTOS/Source/event_groups.c \
../freertos-code/FreeRTOS/Source/list.c \
../freertos-code/FreeRTOS/Source/queue.c \
../freertos-code/FreeRTOS/Source/stream_buffer.c \
../freertos-code/FreeRTOS/Source/tasks.c \
../freertos-code/FreeRTOS/Source/timers.c 

OBJS += \
./freertos-code/FreeRTOS/Source/croutine.o \
./freertos-code/FreeRTOS/Source/event_groups.o \
./freertos-code/FreeRTOS/Source/list.o \
./freertos-code/FreeRTOS/Source/queue.o \
./freertos-code/FreeRTOS/Source/stream_buffer.o \
./freertos-code/FreeRTOS/Source/tasks.o \
./freertos-code/FreeRTOS/Source/timers.o 

C_DEPS += \
./freertos-code/FreeRTOS/Source/croutine.d \
./freertos-code/FreeRTOS/Source/event_groups.d \
./freertos-code/FreeRTOS/Source/list.d \
./freertos-code/FreeRTOS/Source/queue.d \
./freertos-code/FreeRTOS/Source/stream_buffer.d \
./freertos-code/FreeRTOS/Source/tasks.d \
./freertos-code/FreeRTOS/Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
freertos-code/FreeRTOS/Source/%.o: ../freertos-code/FreeRTOS/Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F103xB -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"../freertos-code\FreeRTOS\Source\include" -I"../freertos-code\FreeRTOS\Source\portable\GCC\ARM_CM3" -I../CMSIS_RTOS -I../components/include -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


