@echo off
IF "/%1"=="/name" ECHO:MINGW64 v10.3.0-2 compiler for Windows (32bit and 64 bit) & exit /b 0

call set PATH=C:\TDM-GCC-64\bin;%PATH%
echo:MinGW-x64 Compiler Environment set (v10.3.0-2 32bit and 64bit)
