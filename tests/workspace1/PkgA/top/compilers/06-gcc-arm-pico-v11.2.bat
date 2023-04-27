echo off
IF "/%1"=="/name" ECHO:GCC-ARM compiler for Raspberry Pi Pico & exit /b 0

call set PATH=C:\compilers\gcc-arm-none-eabi\11.2_2022.02\bin;%PATH%
echo:GCC Arm-none-eabi Environment set (v11.2)
