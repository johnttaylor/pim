# UUT
src/Cpl/Io/Serial/ST/M32F4

# Test App
src/Cpl/Io/Serial/_0test/_hw_loopback

# Application Specific OSAL
src/Cpl/System/FreeRTOS/_fatalerror
src/Cpl/System/_trace
src/Cpl/Io/Stdio/_ansi


# CPL Middle-ware
src/Cpl/Container
src/Cpl/Text
src/Cpl/Io/Stdio
src/Cpl/Io

# CPL OSAL
src/Cpl/System
src/Cpl/System/FreeRTOS
src/Cpl/System/_assert


# BSP
src/Bsp/ST/NUCLEO-F413ZH/alpha1 
src/Bsp/ST/NUCLEO-F413ZH/alpha1/MX 
src/Bsp/ST/NUCLEO-F413ZH/alpha1/MX/Core/Src > freertos.c
src/Bsp/ST/NUCLEO-F413ZH/alpha1/console

# SEGGER SysVIEW
src/Bsp/ST/NUCLEO-F413ZH/alpha1/SeggerSysView

# SDK
xsrc/stm32F4-SDK/Drivers/STM32F4xx_HAL_Driver/Src > stm32f4xx_hal_timebase_rtc_alarm_template.c stm32f4xx_hal_timebase_rtc_wakeup_template.c stm32f4xx_hal_timebase_tim_template.c

# FreeRTOS
xsrc/freertos
xsrc/freertos/portable/GCC/ARM_CM4F
