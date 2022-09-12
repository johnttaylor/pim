@echo off
IF "/%1"=="/name" ECHO:GCC-ARM Cortex M/R cross compiler for NRF52/Arduino & exit /b 0

call c:\compilers\nrf52-arduino-5.2.bat
