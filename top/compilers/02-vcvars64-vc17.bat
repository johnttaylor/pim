@echo off
IF "/%1"=="/name" ECHO:Visual Studio VC17 (64bit) compiler for Windows & exit /b 0
::
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
