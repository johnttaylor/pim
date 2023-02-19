@echo off
:: Helper script to copy the UF2 drive to the PICO
::
:: Usage: flash.bat [drive]
::
set _DRIVE=D
IF NOT "/%1"=="/%1" set _DRIVE=%1
copy _pico\*.uf2 %_DRIVE%:\
