################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../components/flash/flash_page_layout.c \
../components/flash/flash_stm32.c \
../components/flash/flash_stm32f1x.c 

OBJS += \
./components/flash/flash_page_layout.o \
./components/flash/flash_stm32.o \
./components/flash/flash_stm32f1x.o 

C_DEPS += \
./components/flash/flash_page_layout.d \
./components/flash/flash_stm32.d \
./components/flash/flash_stm32f1x.d 


# Each subdirectory must supply rules for building sources it contributes
components/flash/%.o: ../components/flash/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F103xB -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"../freertos-code\FreeRTOS\Source\include" -I"../freertos-code\FreeRTOS\Source\portable\GCC\ARM_CM3" -I../CMSIS_RTOS -I../components/include -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


