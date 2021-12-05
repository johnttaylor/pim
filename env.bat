@echo off
:: This script is used to set compiler environment.
::
:: NOTE: The scripts in the top/compilers directory MUST be customized for PC.
::       (only the compilers that you use need to be customized)
::
:: Usage: env [<compiler>]

set _HERE=%~dp0

:: Set helper macros
doskey top=cd %_HERE%
doskey t=cd %_HERE%
doskey bob=%_HERE%xsrc\nqbp\other\bob.py $*
doskey chuck=%_HERE%xsrc\nqbp\other\chuck.py $*
doskey whatcc=echo:%PIM_ENV_COMPILER%
doskey killpy=taskkill /F /IM python.exe


:: Set the NQBP_BIN path
set NQBP_PKG_ROOT=%_HERE%
set NQBP_WORK_ROOT=%_HERE%..\
set NQBP_XPKGS_ROOT=%_HERE%xsrc
set NQBP_BIN=%NQBP_XPKGS_ROOT%\nqbp

:: No compiler option selected
IF "/%1"=="/" GOTO :displaycc


:: Set the compiler toolchain
pushd %_HERE%top
call compiler-list.bat %1
popd
exit /b 0

:: display compiler options
:displaycc
IF "/%PIM_ENV_COMPILER%"=="/"     echo:NO TOOLCHAIN SET
IF NOT "/%PIM_ENV_COMPILER%"=="/" echo:Current toolchain: %PIM_ENV_COMPILER%
echo:
pushd %_HERE%top
call compiler-list.bat
popd
exit /b 0
