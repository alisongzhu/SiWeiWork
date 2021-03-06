################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeModbus/port/freertos/portevent.c \
../FreeModbus/port/freertos/portevent_m.c \
../FreeModbus/port/freertos/portother.c \
../FreeModbus/port/freertos/portserial.c \
../FreeModbus/port/freertos/portserial_m.c \
../FreeModbus/port/freertos/porttimer.c \
../FreeModbus/port/freertos/porttimer_m.c 

OBJS += \
./FreeModbus/port/freertos/portevent.o \
./FreeModbus/port/freertos/portevent_m.o \
./FreeModbus/port/freertos/portother.o \
./FreeModbus/port/freertos/portserial.o \
./FreeModbus/port/freertos/portserial_m.o \
./FreeModbus/port/freertos/porttimer.o \
./FreeModbus/port/freertos/porttimer_m.o 

C_DEPS += \
./FreeModbus/port/freertos/portevent.d \
./FreeModbus/port/freertos/portevent_m.d \
./FreeModbus/port/freertos/portother.d \
./FreeModbus/port/freertos/portserial.d \
./FreeModbus/port/freertos/portserial_m.d \
./FreeModbus/port/freertos/porttimer.d \
./FreeModbus/port/freertos/porttimer_m.d 


# Each subdirectory must supply rules for building sources it contributes
FreeModbus/port/freertos/%.o: ../FreeModbus/port/freertos/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DUSE_FULL_ASSERT -DSTM32F407xx -DUSE_HAL_DRIVER -D_SYS_TIME_H_ -I../Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/posix -I../Middlewares/Third_Party/LwIP/src/include/posix/sys -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -I"../lib60870-C\config" -I"../lib60870-C\src\common\inc" -I"../lib60870-C\src\hal\inc" -I"../lib60870-C\src\inc\api" -I"../lib60870-C\src\inc\internal" -I"../FreeModbus\modbus\include" -I"../FreeModbus\modbus\rtu" -I"../FreeModbus\port" -I../mstp-lib -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


