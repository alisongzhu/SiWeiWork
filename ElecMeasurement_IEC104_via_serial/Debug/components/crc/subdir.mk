################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../components/crc/crc8_sw.c 

OBJS += \
./components/crc/crc8_sw.o 

C_DEPS += \
./components/crc/crc8_sw.d 


# Each subdirectory must supply rules for building sources it contributes
components/crc/%.o: ../components/crc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F103xB -D_SYS_TIME_H_ -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../freertos-code/FreeRTOS/Source/include -I../freertos-code/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../CMSIS_RTOS -I../components/include -I../lib60870-C/config -I../lib60870-C/src/inc/api -I../lib60870-C/src/inc/internal -I../lib60870-C/src/hal/inc -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


