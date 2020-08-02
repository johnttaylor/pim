@echo on
:: This script is used to populate the content that is pulled from other 
:: repositories.
:: NOTES:
::  o All of the repositories ('colony.*', 'pim', etc.) are assumed to be siblings
::    of the same parent directory
::  o This script is run from the ROOT of the 'pim' repository
::
:: Usage: populate.bat [cpr_args]

set _HERE=%~dp0

:: Start with a 'clean slate'
rmdir /S /Q src
rmdir /S /Q xsrc
rmdir /S /Q projects
rmdir /S /Q tests
rmdir /S /Q top\libdirs

:: Copy raw source content
scripts\cpr.py %1 ..\ nqbp xsrc
scripts\cpr.py %1 ..\ ratt xsrc
scripts\cpr.py %1 --filter *.h* ..\catch src .
scripts\cpr.py %1 ..\colony.core src .
::scripts\cpr.py %1 ..\colony.core top\libdirs xsrc colony.core\top\libdirs
scripts\cpr.py %1 ..\colony.bsp src .
scripts\cpr.py %1 ..\colony.core top\libdirs .
scripts\cpr.py %1 ..\colony.apps src .
scripts\cpr.py %1 ..\colony.arduino src .
scripts\cpr.py %1 ..\colony.arduino arduino xsrc colony.arduino\arduino
scripts\cpr.py %1 ..\colony.arduino docs xsrc colony.arduino\docs
scripts\cpr.py %1 ..\colony.arduino top xsrc colony.arduino\top
scripts\cpr.py %1 ..\colony.bsp.renesas.rx src .

scripts\cpr.py %1 --ex tests --ex2 projects --ex3 src ..\ colony.core xsrc


:: Copy tests/projects directories
scripts\cpr.py %1 ..\colony.core tests .
scripts\cpr.py %1 ..\colony.apps tests .
scripts\cpr.py %1 ..\colony.bsp.renesas.rx tests .
scripts\cpr.py %1 ..\colony.apps projects .
scripts\cpr.py %1 ..\colony.arduino tests .
scripts\cpr.py %1 ..\colony.arduino projects .

:: 'Correct' libdirs.b file for the arduino projects
scripts\replace.py --sol tests libdirs.b //colony.core/src/  src/
scripts\replace.py --sol tests libdirs.b //colony.arduino/src/  src/
scripts\replace.py --sol tests libdirs.b arduino/ xsrc/colony.arduino/arduino/
scripts\replace.py --sol tests mytoolchain.py "ARDUINO_SUPPORT = NQBP_PKG_ROOT()" "ARDUINO_SUPPORT = NQBP_PKG_ROOT() + \"\\xsrc\\colony.arduino\""
scripts\replace.py --sol tests mytoolchain.py "COLONY_ARDUINO_BSP_SUPPORT = NQBP_WORK_ROOT() + \"\\xpkgs\\colony.arduino\"" "COLONY_ARDUINO_BSP_SUPPORT = NQBP_PKG_ROOT() 
scripts\replace.py --sol projects libdirs.b //colony.core/src/  src/
scripts\replace.py --sol projects libdirs.b //colony.arduino/src/  src/
scripts\replace.py --sol projects libdirs.b arduino/ xsrc/colony.arduino/arduino/
scripts\replace.py --sol projects mytoolchain.py "ARDUINO_SUPPORT = NQBP_PKG_ROOT()" "ARDUINO_SUPPORT = NQBP_PKG_ROOT() + \"\\xsrc\\colony.arduino\""
scripts\replace.py --sol projects mytoolchain.py "COLONY_ARDUINO_BSP_SUPPORT = NQBP_WORK_ROOT() + \"\\xpkgs\\colony.arduino\"" "COLONY_ARDUINO_BSP_SUPPORT = NQBP_PKG_ROOT() 

:: There can be only one....
del /q /f src\Cpl\MAINPAGE.txt

:: Success
exit /b 0
