::@echo off
:: This script is used to display compiler options and/or set the compiler environment
::
:: NOTE: This script MUST be run in the top/compiler directory!!!!
::
:: usage: compiler-list [<n>]


:: Get the list of available compilers
call compiler-list-helper
IF NOT EXIST _compiler_.txt GOTO :notselected

:: Display list of compilers
IF "/%1"=="/" GOTO :displaylist

:: Configure compilers
set PIM_ENV_COMPILER=no
for /f "tokens=1,2,3* delims=," %%i in (_compiler_.txt) do (
    IF "/%%i"=="/%1" SET PIM_ENV_COMPILER=%%k & call compilers\%%j
)
IF "/%PIM_ENV_COMPILER%"=="/no" GOTO :invalidselection
exit /b 0

:: No available compilers
:notselected
echo:No compilers have-been-configured/are-available.
echo:Check the contents of the top/compilers directory.
exit /b 1

:: Invalid selection
:invalidselection
echo:Invalid compiler selection.  
echo:Try running env.bat without arguments for the list of available compilers.
exit /b 1

:: Dynamically discovered list
:displaylist
for /f "tokens=1,2,3* delims=," %%i in (_compiler_.txt) do echo:%%i - %%k
exit /b 0