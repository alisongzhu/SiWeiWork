################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/lib_AT91SAM7X256.c \
../freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/port.c \
../freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/portISR.c 

OBJS += \
./freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/lib_AT91SAM7X256.o \
./freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/port.o \
./freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/portISR.o 

C_DEPS += \
./freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/lib_AT91SAM7X256.d \
./freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/port.d \
./freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/portISR.d 


# Each subdirectory must supply rules for building sources it contributes
freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/%.o: ../freertos-code/FreeRTOS/Source/portable/GCC/ARM7_AT91SAM7S/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F103xB -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"../freertos-code\FreeRTOS\Source\include" -I"../freertos-code\FreeRTOS\Source\portable\GCC\ARM_CM3" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


