@echo off
:: This is helper script that launches Segger's Ozone debugger
:: The script assumes that Ozone is command path
::
:: usage: debug.bat [ozonefile]

set OFILE=ozone.jdebug
IF NOT "/%1"=="/%1" SET OFILE=%1
start ozone -project %OFILE%
