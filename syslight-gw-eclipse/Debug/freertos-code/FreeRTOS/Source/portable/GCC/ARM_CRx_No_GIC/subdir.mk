################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freertos-code/FreeRTOS/Source/portable/GCC/ARM_CRx_No_GIC/port.c 

S_UPPER_SRCS += \
../freertos-code/FreeRTOS/Source/portable/GCC/ARM_CRx_No_GIC/portASM.S 

OBJS += \
./freertos-code/FreeRTOS/Source/portable/GCC/ARM_CRx_No_GIC/port.o \
./freertos-code/FreeRTOS/Source/portable/GCC/ARM_CRx_No_GIC/portASM.o 

S_UPPER_DEPS += \
./freertos-code/FreeRTOS/Source/portable/GCC/ARM_CRx_No_GIC/portASM.d 

C_DEPS += \
./freertos-code/FreeRTOS/Source/portable/GCC/ARM_CRx_No_GIC/port.d 


# Each subdirectory must supply rules for building sources it contributes
freertos-code/FreeRTOS/Source/portable/GCC/ARM_CRx_No_GIC/%.o: ../freertos-code/FreeRTOS/Source/portable/GCC/ARM_CRx_No_GIC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F103xB -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"../freertos-code\FreeRTOS\Source\include" -I"../freertos-code\FreeRTOS\Source\portable\GCC\ARM_CM3" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-code/FreeRTOS/Source/portable/GCC/ARM_CRx_No_GIC/%.o: ../freertos-code/FreeRTOS/Source/portable/GCC/ARM_CRx_No_GIC/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -x assembler-with-cpp -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F103xB -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"../freertos-code\FreeRTOS\Source\include" -I"../freertos-code\FreeRTOS\Source\portable\GCC\ARM_CM3" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


