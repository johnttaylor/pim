@echo off
:: This script is used to display compiler options and/or set the compiler environment
::
:: NOTE: This script MUST be run in the top/compiler directory!!!!
::
:: usage: master-list [<n>]


:: Display list of compilers
IF "/%1"=="/" GOTO :displaylist

:: Configure compilers
IF "%1"=="1" CALL atsamd51-arduino-v7.bat & echo:SET GCC-ARM Cortex M/R cross compiler for ATSAM51/Arduino
IF "%1"=="2" CALL avr-arduino-v7.3.bat    & echo:SET GCC-AVR cross compiler for Atmel AVR/arduino
IF "%1"=="3" CALL kpit-rx-elf-v15.01.bat  & echo:SET GCC-RX cross compiler for Renesas RX 
IF "%1"=="4" CALL mingw64-v5.1.0.bat      & echo:SET MINGW64 v5.1.0 compiler for Windows
IF "%1"=="5" CALL mingw64-v8.1.0.bat      & echo:SET MINGW64 v8.1.0 compiler for Windows
IF "%1"=="6" CALL nrf52-arduino-v5.2.bat  & echo:SET GCC-ARM Cortex M/R cross compiler for NRF52/Arduino
IF "%1"=="7" CALL vcvars32-vc14.bat       & echo:SET Visual Studio VC14 (32bit) compiler for Windows
IF "%1"=="8" CALL vcvars32-vc16.bat       & echo:SET Visual Studio VC16 (32bit) compiler for Windows
IF "%1"=="9" CALL vcvars64-vc16.bat       & echo:SET Visual Studio VC16 (64bit) compiler for Windows
exit /b 0

:: Hard code list (because it easy and my Windows batch file skills are poor)
:displaylist
echo:1  - GCC-ARM Cortex M/R cross compiler for ATSAM51/Arduino
echo:2  - GCC-AVR cross compiler for Atmel AVR/arduino
echo:3  - GCC-RX cross compiler for Renesas RX 
echo:4  - MINGW64 v5.1.0 compiler for Windows
echo:5  - MINGW64 v8.1.0 compiler for Windows
echo:6  - GCC-ARM Cortex M/R cross compiler for NRF52/Arduino
echo:7  - Visual Studio VC14 (32bit) compiler for Windows
echo:8  - Visual Studio VC16 (32bit) compiler for Windows
echo:9  - Visual Studio VC16 (64bit) compiler for Windows
exit /b 0