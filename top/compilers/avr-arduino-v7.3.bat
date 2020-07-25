@echo off
IF "/%1"=="/name" ECHO:GCC-AVR cross compiler for Atmel AVR/arduino & exit /b 0

:: Core Arduino tools
set NQBP_CMD_OPTIONS=-1
set ARDUINO_TOOLS=c:\compilers\avr-arduino
set ARDUINO_COMPILER_VER=1.8.2
set ARDUINO_BSP_VER=1.8.2

echo:GCC AVR (v7.3), Uno BSP (v%ARDUINO_BSP_VER%) 
