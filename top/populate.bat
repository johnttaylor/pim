@echo on
:: This script is used to populate the content that is pulled from other 
:: repositories.
:: NOTES:
::  o All of the repositories ('colony.*', 'pim', etc.) are assumed to be children
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
scripts\cpr.py %1 ..\catch src\Catch\libs xsrc catch\src\Catch\libs
scripts\cpr.py %1 ..\catch src\Catch\precompiled xsrc catch\src\Catch\precompiled
scripts\cpr.py %1 ..\colony.core src .
scripts\cpr.py %1 ..\colony.core top\libdirs xsrc colony.core\top\libdirs
scripts\cpr.py %1 ..\colony.bsp src .
scripts\cpr.py %1 ..\colony.core top\libdirs .
scripts\cpr.py %1 ..\colony.apps src .
scripts\cpr.py %1 ..\colony.arduino src .
scripts\cpr.py %1 --ex tests --ex2 projects ..\ colony.arduino xsrc


:: Copy tests/projects directories
scripts\cpr.py %1 ..\colony.core tests .
scripts\cpr.py %1 ..\colony.apps tests .
scripts\cpr.py %1 ..\colony.apps projects .


:: Success
exit /b 0
