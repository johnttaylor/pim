::@echo off
:: This script is used to display compiler options and/or set the compiler environment
::
:: NOTE: This script MUST be run in the top/compiler directory!!!!
::
:: usage: master-list [<n>]


:: Display list of compilers
IF "/%1"=="/" GOTO :displaylist

:: Configure compilers
del /q /f _compiler_.txt 1>nul 2>nul
setlocal enableextensions enabledelayedexpansion
set /a index = 0
for /f "delims=" %%i in ('dir /a-d /b compilers\*.bat') do (
    set /a index += 1 
    for /f "delims=" %%j in ('compilers\%%i name') do set COMPILER=%%j
    IF "!index!"=="%1" echo:!index! - !COMPILER! > _compiler_.txt & call compilers\%%i 
)
endlocal
IF NOT EXIST _compiler_.txt GOTO :notselected
for /f "delims=" %%i in (_compiler_.txt) do SET PIM_ENV_COMPILER=%%i
exit /b 0

:notselected
echo:Invalid Compiler selection.  Try 'env.bat' with no options for a list of compilers
exit /b 1

:: Dynamically discovered list
:displaylist
setlocal enableextensions enabledelayedexpansion
set /a index = 0
for /f "delims=" %%i in ('dir /a-d /b compilers\*.bat') do (
    for /f "delims=" %%j in ('compilers\%%i name') do set COMPILER=%%j
    set /a index += 1 
    echo:!index! - !COMPILER!
)
endlocal
exit /b 0