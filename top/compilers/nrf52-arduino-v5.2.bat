@echo off
IF "/%1"=="/name" ECHO:GCC-ARM Cortex M/R cross compiler for NRF52/Arduino & exit /b 0

:: Core Arduino tools
set ARDUINO_TOOLS=c:\compilers\nrf52-arduino-15
set ARDUINO_COMPILER_VER=5_2-2015q4
set ARDUINO_BSP_VER=0.6.0
set ARDUINO_NRF_UTIL_VER=nrfutil-0.5.2

:: Additional/extras
set ARDUINO_SKETCH_FOLDER=c:\compilers\arduino-sketches

:: Force NQBP to be Slow since the gcc-arm ALWAYS seems to crash/reboot my PC!!!!
set NQBP_CMD_OPTIONS=-1

echo:GCC ARM-Cortex M4 (v%ARDUINO_COMPILER_VER%), nRF52 Feather BSP (v%ARDUINO_BSP_VER%)
