echo off
IF "/%1"=="/name" ECHO:GCC-ARM (none-eabi) compiler for STM32 & exit /b 0

:: Segger support
set PATH=%PATH%;c:\Program Files (x86)\SEGGER\JLink
set PATH=%PATH%;c:\Program Files\SEGGER\Ozone

call set PATH=%NQBP_XPKGS_ROOT%\stm32-gcc-arm\bin;%PATH%
echo:GCC Arm-none-eabi Environment set to xsrc\stm32-gcc-arm\bin