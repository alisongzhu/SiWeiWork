################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeModbus/modbus/functions/mbfunccoils.c \
../FreeModbus/modbus/functions/mbfunccoils_m.c \
../FreeModbus/modbus/functions/mbfuncdiag.c \
../FreeModbus/modbus/functions/mbfuncdisc.c \
../FreeModbus/modbus/functions/mbfuncdisc_m.c \
../FreeModbus/modbus/functions/mbfuncholding.c \
../FreeModbus/modbus/functions/mbfuncholding_m.c \
../FreeModbus/modbus/functions/mbfuncinput.c \
../FreeModbus/modbus/functions/mbfuncinput_m.c \
../FreeModbus/modbus/functions/mbfuncother.c \
../FreeModbus/modbus/functions/mbutils.c 

OBJS += \
./FreeModbus/modbus/functions/mbfunccoils.o \
./FreeModbus/modbus/functions/mbfunccoils_m.o \
./FreeModbus/modbus/functions/mbfuncdiag.o \
./FreeModbus/modbus/functions/mbfuncdisc.o \
./FreeModbus/modbus/functions/mbfuncdisc_m.o \
./FreeModbus/modbus/functions/mbfuncholding.o \
./FreeModbus/modbus/functions/mbfuncholding_m.o \
./FreeModbus/modbus/functions/mbfuncinput.o \
./FreeModbus/modbus/functions/mbfuncinput_m.o \
./FreeModbus/modbus/functions/mbfuncother.o \
./FreeModbus/modbus/functions/mbutils.o 

C_DEPS += \
./FreeModbus/modbus/functions/mbfunccoils.d \
./FreeModbus/modbus/functions/mbfunccoils_m.d \
./FreeModbus/modbus/functions/mbfuncdiag.d \
./FreeModbus/modbus/functions/mbfuncdisc.d \
./FreeModbus/modbus/functions/mbfuncdisc_m.d \
./FreeModbus/modbus/functions/mbfuncholding.d \
./FreeModbus/modbus/functions/mbfuncholding_m.d \
./FreeModbus/modbus/functions/mbfuncinput.d \
./FreeModbus/modbus/functions/mbfuncinput_m.d \
./FreeModbus/modbus/functions/mbfuncother.d \
./FreeModbus/modbus/functions/mbutils.d 


# Each subdirectory must supply rules for building sources it contributes
FreeModbus/modbus/functions/%.o: ../FreeModbus/modbus/functions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g -DSTM32F407xx -DUSE_HAL_DRIVER -D_SYS_TIME_H_ -I../Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/posix -I../Middlewares/Third_Party/LwIP/src/include/posix/sys -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -I"../lib60870-C\config" -I"../lib60870-C\src\common\inc" -I"../lib60870-C\src\hal\inc" -I"../lib60870-C\src\inc\api" -I"../lib60870-C\src\inc\internal" -I"../FreeModbus\modbus\include" -I"../FreeModbus\modbus\rtu" -I"../FreeModbus\port" -I../mstp-lib -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


