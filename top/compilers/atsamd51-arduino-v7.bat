@echo off
IF "/%1"=="/name" ECHO:GCC-ARM Cortex M/R cross compiler for ATSAM51/Arduino & exit /b 0

:: Core Arduino tools
set ARDUINO_TOOLS=C:\Users\User\AppData\Local\Arduino15\packages\adafruit
set ARDUINO_COMPILER_VER=7-2017q4
set ARDUINO_BSP_VER=1.5.7

:: Additional/extras
set SEGGER_TOOLS_JLINK="e:\Program Files (x86)\SEGGER\JLink"
set SEGGER_TOOLS_OZONE="e:\Program Files\SEGGER\Ozone V3.10a"

echo:GCC ARM-Cortex M4 (%ARDUINO_COMPILER_VER%), Adafruit's Grand Central BSP (v%ARDUINO_BSP_VER%) setup
