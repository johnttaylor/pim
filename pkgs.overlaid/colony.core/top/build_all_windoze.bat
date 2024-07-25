::@echo off
:: This script is used by the CI\Build machine to build the Windows test
:: projects
::
:: usage: build_all_windoze.bat <buildNumber> [branch]
::

set _TOPDIR=%~dp0
set _ROOT=%_TOPDIR%..


:: Set Build info (and force build number to zero for "non-official" builds)
set BUILD_TYPE=%2
set BUILD_NUMBER=%1
IF "%BUILD_TYPE%"=="pr" set BUILD_NUMBER=0
IF "%BUILD_TYPE%"=="PR" set BUILD_NUMBER=0
IF "%BUILD_TYPE%"=="unknown" set BUILD_NUMBER=0
IF "/%BUILD_TYPE%" =="/" set BUILD_TYPE=private
echo:
echo:BUILD TYPE=%BUILD_TYPE%, BUILD_NUMBER=%BUILD_NUMBER%
echo:

:: Make sure the _artifacts directory exists and is empty
cd %_ROOT%
rmdir /s /q _artifacts
mkdir _artifacts

:: Run Doxygen first 
cd %_TOPDIR%
run_doxygen.py %BUILD_TYPE% %BUILD_NUMBER%
IF ERRORLEVEL 1 EXIT /b 1
copy %_ROOT%\docs\colony.core-library_%BUILD_NUMBER%-%BUILD_TYPE%.chm %_ROOT%\_artifacts\colony.core-library__%BUILD_NUMBER%-%BUILD_TYPE%.chm
IF ERRORLEVEL 1 EXIT /b 1

::
:: Build Mingw projects
::
echo on
call %_ROOT%\env.bat 3

cd %_ROOT%\tests
%_ROOT%\xsrc\nqbp2\other\bob.py -v4 mingw_w64 --bldtime --bld-all --bldnum %BUILD_NUMBER%
IF ERRORLEVEL 1 EXIT /b 1

:: Run unit tests
cd %_TOPDIR%\..\tests
%_ROOT%\xsrc\nqbp2\other\chuck.py -vt --match a.exe --dir mingw_w64
IF ERRORLEVEL 1 EXIT /b 1
%_ROOT%\xsrc\nqbp2\other\chuck.py -v --match aa.exe --dir mingw_w64
IF ERRORLEVEL 1 EXIT /b 1
%_ROOT%\xsrc\nqbp2\other\chuck.py -vt --match a.py --dir mingw_w64
IF ERRORLEVEL 1 EXIT /b 1
%_ROOT%\xsrc\nqbp2\other\chuck.py -v --match aa.py --dir mingw_w64
IF ERRORLEVEL 1 EXIT /b 1

:: Generate code coverage metrics
%_ROOT%\xsrc\nqbp2\other\chuck.py -v --dir mingw_w64 --match tca.py args --ci rpt --xml jenkins-gcovr.xml 
IF ERRORLEVEL 1 EXIT /b 1


::
:: Build Visual Studio projects
::
echo on
call %_TOPDIR%..\env.bat 1

cd %_TOPDIR%..\tests
%_ROOT%\xsrc\nqbp2\other\bob.py -v4 vc12 --bldtime --bld-all --bldnum %BUILD_NUMBER%
IF ERRORLEVEL 1 EXIT /b 1

:: Run unit tests
cd %_TOPDIR%\..\tests
%_ROOT%\xsrc\nqbp2\other\chuck.py -vt --match a.exe --dir vc12
IF ERRORLEVEL 1 EXIT /b 1
%_ROOT%\xsrc\nqbp2\other\chuck.py -v --match aa.exe --dir vc12
IF ERRORLEVEL 1 EXIT /b 1
%_ROOT%\xsrc\nqbp2\other\chuck.py -vt --match a.py --dir vc12
IF ERRORLEVEL 1 EXIT /b 1
%_ROOT%\xsrc\nqbp2\other\chuck.py -v --match aa.py --dir vc12
IF ERRORLEVEL 1 EXIT /b 1

::
:: Everything worked!
::
exit /b 0
