@echo off
:: Check environment variable
IF "/"%SEGGER_TOOLS_JLINK%=="/" GOTO :help

:: Script to program the .BIN using Segger's JLINK 
%SEGGER_TOOLS_JLINK%\JFlash.exe -openprjatsamd51P20-no-bootloader.jflash -open_arduino\a.bin -auto -exit
goto :exit

:: Error message about missing environment setup
:help
echo:The environment variable SEGGER_TOOLS_JLINK has not been set

:: Single exit point
:exit


