################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Modbus/port/freertos/portevent.c \
../Modbus/port/freertos/portevent_m.c \
../Modbus/port/freertos/portother.c \
../Modbus/port/freertos/portserial.c \
../Modbus/port/freertos/portserial_m.c \
../Modbus/port/freertos/porttimer.c \
../Modbus/port/freertos/porttimer_m.c 

OBJS += \
./Modbus/port/freertos/portevent.o \
./Modbus/port/freertos/portevent_m.o \
./Modbus/port/freertos/portother.o \
./Modbus/port/freertos/portserial.o \
./Modbus/port/freertos/portserial_m.o \
./Modbus/port/freertos/porttimer.o \
./Modbus/port/freertos/porttimer_m.o 

C_DEPS += \
./Modbus/port/freertos/portevent.d \
./Modbus/port/freertos/portevent_m.d \
./Modbus/port/freertos/portother.d \
./Modbus/port/freertos/portserial.d \
./Modbus/port/freertos/portserial_m.d \
./Modbus/port/freertos/porttimer.d \
./Modbus/port/freertos/porttimer_m.d 


# Each subdirectory must supply rules for building sources it contributes
Modbus/port/freertos/%.o: ../Modbus/port/freertos/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wall -Wextra  -g3 -DUSE_FULL_ASSERT -DSTM32F407xx -DUSE_HAL_DRIVER -I../Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/posix -I../Middlewares/Third_Party/LwIP/src/include/posix/sys -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -I"../lib60870-C\config" -I"../lib60870-C\src\common\inc" -I"../lib60870-C\src\hal\inc" -I"../lib60870-C\src\inc\api" -I"../lib60870-C\src\inc\internal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


