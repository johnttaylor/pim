@echo off
IF "/%1"=="/name" ECHO:Visual Studio VC16 (64bit) compiler for Windows & exit /b 0
::
call "C:\compilers\vcvars64-vc16.bat"


