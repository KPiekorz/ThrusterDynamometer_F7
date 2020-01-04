################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/controlpanel.c \
../Src/ethernetif.c \
../Src/freertos.c \
../Src/lwip.c \
../Src/main.c \
../Src/stm32f7xx_hal_msp.c \
../Src/stm32f7xx_hal_timebase_tim.c \
../Src/stm32f7xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f7xx.c \
../Src/tenso.c \
../Src/usb_device.c \
../Src/usbd_cdc_if.c \
../Src/usbd_conf.c \
../Src/usbd_desc.c 

OBJS += \
./Src/controlpanel.o \
./Src/ethernetif.o \
./Src/freertos.o \
./Src/lwip.o \
./Src/main.o \
./Src/stm32f7xx_hal_msp.o \
./Src/stm32f7xx_hal_timebase_tim.o \
./Src/stm32f7xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f7xx.o \
./Src/tenso.o \
./Src/usb_device.o \
./Src/usbd_cdc_if.o \
./Src/usbd_conf.o \
./Src/usbd_desc.o 

C_DEPS += \
./Src/controlpanel.d \
./Src/ethernetif.d \
./Src/freertos.d \
./Src/lwip.d \
./Src/main.d \
./Src/stm32f7xx_hal_msp.d \
./Src/stm32f7xx_hal_timebase_tim.d \
./Src/stm32f7xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f7xx.d \
./Src/tenso.d \
./Src/usb_device.d \
./Src/usbd_cdc_if.d \
./Src/usbd_conf.d \
./Src/usbd_desc.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F767xx -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/SEGGER/Config" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/SEGGER/OS" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/SEGGER/SEGGER" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/FreeRTOS/org/Source/include" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/FreeRTOS/org/Source/CMSIS_RTOS" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/FreeRTOS/org/Source/portable/GCC/ARM_CM7/r0p1" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Inc" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/src/include" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/system" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/src/include/netif/ppp" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/src/include/lwip" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/src/include/lwip/apps" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/src/include/lwip/priv" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/src/include/lwip/prot" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/src/include/netif" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/src/include/posix" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/src/include/posix/sys" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/Third_Party/LwIP/system/arch" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Drivers/CMSIS/Include" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"D:/ThrusterDynamometer/ThrusterDynamometer_F7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


