::@echo off
IF "/%1"=="/name" ECHO:Visual Studio VC17 (32bit) compiler for Windows & exit /b 0

call "C:\compilers\vcvars32-vc17.bat"
