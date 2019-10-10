################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib60870-C/src/iec60870/apl/asdu.c \
../lib60870-C/src/iec60870/apl/bcr.c \
../lib60870-C/src/iec60870/apl/cpXXtime2a.c \
../lib60870-C/src/iec60870/apl/information_objects.c 

OBJS += \
./lib60870-C/src/iec60870/apl/asdu.o \
./lib60870-C/src/iec60870/apl/bcr.o \
./lib60870-C/src/iec60870/apl/cpXXtime2a.o \
./lib60870-C/src/iec60870/apl/information_objects.o 

C_DEPS += \
./lib60870-C/src/iec60870/apl/asdu.d \
./lib60870-C/src/iec60870/apl/bcr.d \
./lib60870-C/src/iec60870/apl/cpXXtime2a.d \
./lib60870-C/src/iec60870/apl/information_objects.d 


# Each subdirectory must supply rules for building sources it contributes
lib60870-C/src/iec60870/apl/%.o: ../lib60870-C/src/iec60870/apl/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DSTM32F407xx -DUSE_HAL_DRIVER -D_SYS_TIME_H_ -I../Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/posix -I../Middlewares/Third_Party/LwIP/src/include/posix/sys -I../Drivers/CMSIS/Include -I"../Middlewares\Third_Party\LwIP\system\arch" -I"../Middlewares\Third_Party\FreeRTOS\Source\CMSIS_RTOS" -I../FreeModbus/modbus/include -I../FreeModbus/modbus/rtu -I../FreeModbus/port -I../lib60870-C/config -I../lib60870-C/src/common/inc -I../lib60870-C/src/hal/inc -I../lib60870-C/src/inc/api -I../lib60870-C/src/inc/internal -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


