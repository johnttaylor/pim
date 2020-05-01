@echo on
:: This script CLEANS the SOURCE directories and then it calls the populdate.bat
:: script
::
:: Usage: clean-and-populate.bat [cpr_args]

set _HERE=%~dp0

:: Require the NQBP_BIN environment variable to be set
set NQBP_BIN=%_HERE%..\..\nqbp

:: Clean the source tests and projects directory
set NQBP_XPKG_MODEL=outcast
pushd ..\colony.core\tests
..\..\nqbp\other\bob.py here -z
popd
pushd ..\colony.apps\projects
..\..\nqbp\other\bob.py here -z
popd
pushd ..\colony.apps\tests
..\..\nqbp\other\bob.py here -z
popd
pushd ..\colony.arduino\tests
..\..\nqbp\other\bob.py here -z
popd
pushd ..\colony.arduino\projects
..\..\nqbp\other\bob.py here -z
popd

:: Call the populate script
::%_HERE%populate.bat
