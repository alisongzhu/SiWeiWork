################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Modbus/modbus/functions/mbfunccoils.c \
../Modbus/modbus/functions/mbfunccoils_m.c \
../Modbus/modbus/functions/mbfuncdiag.c \
../Modbus/modbus/functions/mbfuncdisc.c \
../Modbus/modbus/functions/mbfuncdisc_m.c \
../Modbus/modbus/functions/mbfuncholding.c \
../Modbus/modbus/functions/mbfuncholding_m.c \
../Modbus/modbus/functions/mbfuncinput.c \
../Modbus/modbus/functions/mbfuncinput_m.c \
../Modbus/modbus/functions/mbfuncother.c \
../Modbus/modbus/functions/mbutils.c 

OBJS += \
./Modbus/modbus/functions/mbfunccoils.o \
./Modbus/modbus/functions/mbfunccoils_m.o \
./Modbus/modbus/functions/mbfuncdiag.o \
./Modbus/modbus/functions/mbfuncdisc.o \
./Modbus/modbus/functions/mbfuncdisc_m.o \
./Modbus/modbus/functions/mbfuncholding.o \
./Modbus/modbus/functions/mbfuncholding_m.o \
./Modbus/modbus/functions/mbfuncinput.o \
./Modbus/modbus/functions/mbfuncinput_m.o \
./Modbus/modbus/functions/mbfuncother.o \
./Modbus/modbus/functions/mbutils.o 

C_DEPS += \
./Modbus/modbus/functions/mbfunccoils.d \
./Modbus/modbus/functions/mbfunccoils_m.d \
./Modbus/modbus/functions/mbfuncdiag.d \
./Modbus/modbus/functions/mbfuncdisc.d \
./Modbus/modbus/functions/mbfuncdisc_m.d \
./Modbus/modbus/functions/mbfuncholding.d \
./Modbus/modbus/functions/mbfuncholding_m.d \
./Modbus/modbus/functions/mbfuncinput.d \
./Modbus/modbus/functions/mbfuncinput_m.d \
./Modbus/modbus/functions/mbfuncother.d \
./Modbus/modbus/functions/mbutils.d 


# Each subdirectory must supply rules for building sources it contributes
Modbus/modbus/functions/%.o: ../Modbus/modbus/functions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wall -Wextra  -g3 -DUSE_FULL_ASSERT -DSTM32F407xx -DUSE_HAL_DRIVER -I../Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/posix -I../Middlewares/Third_Party/LwIP/src/include/posix/sys -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -I"../lib60870-C\config" -I"../lib60870-C\src\common\inc" -I"../lib60870-C\src\hal\inc" -I"../lib60870-C\src\inc\api" -I"../lib60870-C\src\inc\internal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


