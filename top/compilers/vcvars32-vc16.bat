@echo off
IF "/%1"=="/name" ECHO:Visual Studio VC16 (32bit) compiler for Windows & exit /b 0

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
