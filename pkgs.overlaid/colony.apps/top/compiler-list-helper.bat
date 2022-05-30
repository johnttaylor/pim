@echo off
:: This is helper script that generates a list of compilers in the top/compilers 
:: directory
:: usage: compiler-list-helper 
:: Returns: <index>, <filename>, <name>


:: Configure compilers
del /q /f _compiler_.txt 1>nul 2>nul
setlocal enableextensions enabledelayedexpansion
set /a index = 0
for /f "delims=" %%i in ('dir /a-d /b compilers\*.bat') do (
    set /a index += 1 
    for /f "delims=" %%j in ('compilers\%%i name') do set COMPILER=%%j
    echo:!index!,%%i,!COMPILER! >> _compiler_.txt
)
endlocal
exit /b 0