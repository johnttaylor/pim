@echo off
IF "/%1"=="/name" ECHO:MINGW64 v9.2.0 compiler for Windows (32bit and 64 bit) & exit /b 0

call set PATH=C:\compilers\TDM-GCC-64\bin;%PATH%
echo:MinGW-x64 Compiler Environment set (v9.2.0 - 32bit and 64bit)

