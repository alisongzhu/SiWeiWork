################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include mstp-lib/802.1Q-2011/subdir.mk
-include mstp-lib/subdir.mk
-include lib60870-C/src/iec60870/t104/subdir.mk
-include lib60870-C/src/iec60870/apl/subdir.mk
-include lib60870-C/src/iec60870/subdir.mk
-include lib60870-C/src/hal/time/freertos/subdir.mk
-include lib60870-C/src/hal/thread/freertos/subdir.mk
-include lib60870-C/src/hal/socket/freertos/subdir.mk
-include lib60870-C/src/common/subdir.mk
-include Src/subdir.mk
-include Middlewares/Third_Party/LwIP/system/OS/subdir.mk
-include Middlewares/Third_Party/LwIP/src/netif/ppp/subdir.mk
-include Middlewares/Third_Party/LwIP/src/netif/subdir.mk
-include Middlewares/Third_Party/LwIP/src/core/ipv6/subdir.mk
-include Middlewares/Third_Party/LwIP/src/core/ipv4/subdir.mk
-include Middlewares/Third_Party/LwIP/src/core/subdir.mk
-include Middlewares/Third_Party/LwIP/src/apps/tftp/subdir.mk
-include Middlewares/Third_Party/LwIP/src/api/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include FreeModbus/port/freertos/subdir.mk
-include FreeModbus/port/subdir.mk
-include FreeModbus/modbus/tcp/subdir.mk
-include FreeModbus/modbus/rtu/subdir.mk
-include FreeModbus/modbus/functions/subdir.mk
-include FreeModbus/modbus/ascii/subdir.mk
-include FreeModbus/modbus/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
iTU-FX-RING-GCC.hex \

SECONDARY_SIZE += \
iTU-FX-RING-GCC.siz \


# All Target
all: iTU-FX-RING-GCC.elf secondary-outputs

# Tool invocations
iTU-FX-RING-GCC.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM Cross C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -T "E:\develop\eclipse-workspace\iTU-FX-RING-GCC\STM32F407VGTx_FLASH.ld" -Xlinker --gc-sections -Wl,-Map,"iTU-FX-RING-GCC.map" -Xlinker --cref --specs=nano.specs -o "iTU-FX-RING-GCC.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

iTU-FX-RING-GCC.hex: iTU-FX-RING-GCC.elf
	@echo 'Invoking: GNU ARM Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex "iTU-FX-RING-GCC.elf"  "iTU-FX-RING-GCC.hex"
	@echo 'Finished building: $@'
	@echo ' '

iTU-FX-RING-GCC.siz: iTU-FX-RING-GCC.elf
	@echo 'Invoking: GNU ARM Cross Print Size'
	arm-none-eabi-size --format=berkeley "iTU-FX-RING-GCC.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS)$(CPP_DEPS) iTU-FX-RING-GCC.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
