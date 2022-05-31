::@echo off
IF "/%1"=="/name" ECHO:Visual Studio VC16 (32bit) compiler for Windows & exit /b 0

call "C:\compilers\vcvars32-vc16.bat"
