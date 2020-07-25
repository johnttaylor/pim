@echo off
IF "/%1"=="/name" ECHO:MINGW64 v5.1.0 compiler for Windows & exit /b 0

set PATH=C:\compilers\mingw64-v5.1.0\bin;%PATH%
echo:MinGW-x64 Compiler Environment set (v5.1.0)
