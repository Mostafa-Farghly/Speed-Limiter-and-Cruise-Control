################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include FreeRTOS/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
SpeedLimiter_and_CruiseControl.lss \

FLASH_IMAGE += \
SpeedLimiter_and_CruiseControl.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: SpeedLimiter_and_CruiseControl.elf secondary-outputs

# Tool invocations
SpeedLimiter_and_CruiseControl.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,SpeedLimiter_and_CruiseControl.map -mmcu=atmega128 -o "SpeedLimiter_and_CruiseControl.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

SpeedLimiter_and_CruiseControl.lss: SpeedLimiter_and_CruiseControl.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S SpeedLimiter_and_CruiseControl.elf  >"SpeedLimiter_and_CruiseControl.lss"
	@echo 'Finished building: $@'
	@echo ' '

SpeedLimiter_and_CruiseControl.hex: SpeedLimiter_and_CruiseControl.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex SpeedLimiter_and_CruiseControl.elf  "SpeedLimiter_and_CruiseControl.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: SpeedLimiter_and_CruiseControl.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega128 SpeedLimiter_and_CruiseControl.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) SpeedLimiter_and_CruiseControl.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents

-include ../makefile.targets
