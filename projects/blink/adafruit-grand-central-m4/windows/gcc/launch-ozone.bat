@echo off
:: Check environment variable
IF "/"%SEGGER_TOOLS_OZONE%=="/" GOTO :help

:: Script to launch OZONE debugger
start "Ozone" %SEGGER_TOOLS_OZONE%\ozone.exe -project ozone-debugger.jdebug
goto :exit

:: Error message about missing environment setup
:help
echo:The environment variable SEGGER_TOOLS_OZONE has not been set

:: Single exit point
:exit


