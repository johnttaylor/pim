echo off
IF "/%1"=="/name" ECHO:GCC-ARM compiler for Grand Central BSP 1.6.0 & exit /b 0

:: Core Arduino tools
set ARDUINO_BSP_VER=1.6.0

:: Segger support
set PATH=%PATH%;c:\Program Files (x86)\SEGGER\JLink
set PATH=%PATH%;c:\Program Files\SEGGER\Ozone

:: Add Compiler to the path. Note: Leverage the same cross compiler used for the STM32 MCUs
call set PATH=%NQBP_XPKGS_ROOT%\stm32-gcc-arm\bin;%PATH%

::
echo:Environment set for ARM-Cortex M4, Adafruit's Grand Central BSP (v%ARDUINO_BSP_VER%)
