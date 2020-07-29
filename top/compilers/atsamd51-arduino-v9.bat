echo off
IF "/%1"=="/name" ECHO:GCC-ARM compiler for Grand Central BSP 1.6.0 & exit /b 0

:: Core Arduino tools
set ARDUINO_TOOLS=C:\Users\johnt\AppData\Local\Arduino15\packages\adafruit
set ARDUINO_COMPILER_VER=9-2019q4
set ARDUINO_BSP_VER=1.6.0

:: Additional/extras
set SEGGER_TOOLS_JLINK="c:\Program Files (x86)\SEGGER\JLink"
set SEGGER_TOOLS_OZONE="c:\Program Files\SEGGER\Ozone"

echo:GCC ARM-Cortex M4 (%ARDUINO_COMPILER_VER%), Adafruit's Grand Central BSP (v%ARDUINO_BSP_VER%) setup
