################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/DM8606C.c \
../Src/can.c \
../Src/can_network.c \
../Src/crc.c \
../Src/dm8606c_stp.c \
../Src/dma.c \
../Src/dtu.c \
../Src/env.c \
../Src/ethernetif.c \
../Src/flash_if.c \
../Src/freertos.c \
../Src/gpio.c \
../Src/iec104_handler.c \
../Src/iwdg.c \
../Src/lwip.c \
../Src/main.c \
../Src/retarget.c \
../Src/rs485_network.c \
../Src/rtc.c \
../Src/spi.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_hal_timebase_TIM.c \
../Src/stm32f4xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f4xx.c \
../Src/tim.c \
../Src/usart.c \
../Src/user_config_raw.c 

OBJS += \
./Src/DM8606C.o \
./Src/can.o \
./Src/can_network.o \
./Src/crc.o \
./Src/dm8606c_stp.o \
./Src/dma.o \
./Src/dtu.o \
./Src/env.o \
./Src/ethernetif.o \
./Src/flash_if.o \
./Src/freertos.o \
./Src/gpio.o \
./Src/iec104_handler.o \
./Src/iwdg.o \
./Src/lwip.o \
./Src/main.o \
./Src/retarget.o \
./Src/rs485_network.o \
./Src/rtc.o \
./Src/spi.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_hal_timebase_TIM.o \
./Src/stm32f4xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f4xx.o \
./Src/tim.o \
./Src/usart.o \
./Src/user_config_raw.o 

C_DEPS += \
./Src/DM8606C.d \
./Src/can.d \
./Src/can_network.d \
./Src/crc.d \
./Src/dm8606c_stp.d \
./Src/dma.d \
./Src/dtu.d \
./Src/env.d \
./Src/ethernetif.d \
./Src/flash_if.d \
./Src/freertos.d \
./Src/gpio.d \
./Src/iec104_handler.d \
./Src/iwdg.d \
./Src/lwip.d \
./Src/main.d \
./Src/retarget.d \
./Src/rs485_network.d \
./Src/rtc.d \
./Src/spi.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_hal_timebase_TIM.d \
./Src/stm32f4xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f4xx.d \
./Src/tim.d \
./Src/usart.d \
./Src/user_config_raw.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DUSE_FULL_ASSERT -DSTM32F407xx -DUSE_HAL_DRIVER -D_SYS_TIME_H_ -I../Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/posix -I../Middlewares/Third_Party/LwIP/src/include/posix/sys -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -I"../lib60870-C\config" -I"../lib60870-C\src\common\inc" -I"../lib60870-C\src\hal\inc" -I"../lib60870-C\src\inc\api" -I"../lib60870-C\src\inc\internal" -I"../FreeModbus\modbus\include" -I"../FreeModbus\modbus\rtu" -I"../FreeModbus\port" -I../mstp-lib -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


