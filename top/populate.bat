@echo on
:: This script is used to populate the content that is pulled from other 
:: repositories.
:: NOTES:
::  o All of the repositories ('colony.*', 'pim', etc.) are assumed to be children
::    of the same parent directory
::  o This script is run from the ROOT of the 'pim' repository
::
:: Usage: populate.bat [cpr_args]

:: Require the NQBP_BIN environment variable to be set
if "/%NQBP_BIN%"=="/" goto :nqbperror

:: Start with a 'clean slate'
rmdir /S /Q src
rmdir /S /Q arduino
rmdir /S /Q xsrc
rmdir /S /Q projects
rmdir /S /Q tests

:: Copy raw source content
scripts\cpr.py %1 ..\ nqbp xsrc
scripts\cpr.py %1 ..\ ratt xsrc
scripts\cpr.py %1 ..\catch\src Catch xsrc Catch
scripts\cpr.py %1 ..\colony.core src .
scripts\cpr.py %1 ..\colony.core top\libdirs .
scripts\cpr.py %1 ..\colony.apps src .
scripts\cpr.py %1 ..\colony.arduino src .
scripts\cpr.py %1 ..\colony.arduino arduino .


:: Clean the source tests and projects directory
::set NQBP_OUTCAST_MODE=false
::pushd ..\colony.core\tests
::..\..\pim\xsrc\nqbp\other\bob.py here -z
::popd
::pushd ..\colony.apps\projects
::..\..\pim\xsrc\nqbp\other\bob.py here -z
::popd
::pushd ..\colony.apps\tests
::..\..\pim\xsrc\nqbp\other\bob.py here -z
::popd
::pushd ..\colony.arduino\tests
::..\..\pim\xsrc\nqbp\other\bob.py here -z
::popd
::pushd ..\colony.arduino\projects
::..\..\pim\xsrc\nqbp\other\bob.py here -z
::popd

:: Copy tests/projects directories
scripts\cpr.py %1 ..\colony.core tests .
scripts\cpr.py %1 ..\colony.apps tests .
scripts\cpr.py %1 ..\colony.apps projects .
scripts\cpr.py %1 ..\colony.arduino tests .
scripts\cpr.py %1 ..\colony.arduino projects .


:: Success
exit /b 0

:nqbperror
echo:The NQBP_BIN environment variable is NOT set
exit /b 1