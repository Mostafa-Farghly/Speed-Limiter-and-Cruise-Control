################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/DIO_program.c \
../drivers/usart_driver.c 

OBJS += \
./drivers/DIO_program.o \
./drivers/usart_driver.o 

C_DEPS += \
./drivers/DIO_program.d \
./drivers/usart_driver.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"D:\NTI\RTOS\workspaces\AVRWorkspace\HelperMicro\ConfigHeaders" -I"D:\NTI\RTOS\workspaces\AVRWorkspace\HelperMicro\driversHeaders" -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega128 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


