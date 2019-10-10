################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../iec104/src/iec60870/apl/asdu.c \
../iec104/src/iec60870/apl/bcr.c \
../iec104/src/iec60870/apl/cpXXtime2a.c \
../iec104/src/iec60870/apl/information_objects.c 

OBJS += \
./iec104/src/iec60870/apl/asdu.o \
./iec104/src/iec60870/apl/bcr.o \
./iec104/src/iec60870/apl/cpXXtime2a.o \
./iec104/src/iec60870/apl/information_objects.o 

C_DEPS += \
./iec104/src/iec60870/apl/asdu.d \
./iec104/src/iec60870/apl/bcr.d \
./iec104/src/iec60870/apl/cpXXtime2a.d \
./iec104/src/iec60870/apl/information_objects.d 


# Each subdirectory must supply rules for building sources it contributes
iec104/src/iec60870/apl/%.o: ../iec104/src/iec60870/apl/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wall -Wextra  -g3 -DUSE_FULL_ASSERT -DSTM32F407xx -DUSE_HAL_DRIVER -I../Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/posix -I../Middlewares/Third_Party/LwIP/src/include/posix/sys -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -I"../lib60870-C\config" -I"../lib60870-C\src\common\inc" -I"../lib60870-C\src\hal\inc" -I"../lib60870-C\src\inc\api" -I"../lib60870-C\src\inc\internal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


