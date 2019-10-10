################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../mstp/802.1Q-2011/802_1Q_2011_SM.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_BridgeDetection.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_L2GPReceive.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_PortInformation.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_PortProtocolMigration.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_PortReceive.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_PortRoleSelection.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_PortRoleTransitions.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_PortStateTransition.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_PortTimers.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_PortTransmit.cpp \
../mstp/802.1Q-2011/802_1Q_2011_SM_TopologyChange.cpp \
../mstp/802.1Q-2011/802_1Q_2011_procedures.cpp 

OBJS += \
./mstp/802.1Q-2011/802_1Q_2011_SM.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_BridgeDetection.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_L2GPReceive.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortInformation.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortProtocolMigration.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortReceive.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortRoleSelection.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortRoleTransitions.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortStateTransition.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortTimers.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortTransmit.o \
./mstp/802.1Q-2011/802_1Q_2011_SM_TopologyChange.o \
./mstp/802.1Q-2011/802_1Q_2011_procedures.o 

CPP_DEPS += \
./mstp/802.1Q-2011/802_1Q_2011_SM.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_BridgeDetection.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_L2GPReceive.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortInformation.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortProtocolMigration.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortReceive.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortRoleSelection.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortRoleTransitions.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortStateTransition.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortTimers.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_PortTransmit.d \
./mstp/802.1Q-2011/802_1Q_2011_SM_TopologyChange.d \
./mstp/802.1Q-2011/802_1Q_2011_procedures.d 


# Each subdirectory must supply rules for building sources it contributes
mstp/802.1Q-2011/%.o: ../mstp/802.1Q-2011/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wall -Wextra  -g3 -DUSE_FULL_ASSERT -DSTM32F407xx -DUSE_HAL_DRIVER -I../Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/posix -I../Middlewares/Third_Party/LwIP/src/include/posix/sys -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -I"../lib60870-C\config" -I"../lib60870-C\src\common\inc" -I"../lib60870-C\src\hal\inc" -I"../lib60870-C\src\inc\api" -I"../lib60870-C\src\inc\internal" -I"../FreeModbus\modbus\include" -I"../FreeModbus\modbus\rtu" -I"../FreeModbus\port" -I../mstp-lib -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


