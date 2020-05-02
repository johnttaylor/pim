@echo on
:: Build script for Jenkins
::
:: Usage: ci_jenkins_build.bat [<branch>]

:: Cache branch info
set TOP_DIR=%~p0
set PKG_BRANCH=%1

:: Calculate the build number (is the total number of commits acrossed master and develop branches in the Git repo)
pushd colony.core
for /f %%i in ('git rev-list --count origin/master') do set MASTER_COUNT=%%i
for /f %%i in ('git rev-list --count origin/develop') do set DEVELOP_COUNT=%%i
popd
set /A BLDNUM=%MASTER_COUNT%+%DEVELOP_COUNT%
echo:BuildNumber=%BLDNUM% > build_number_%BLDNUM%.txt
pushd colony.core
git log --max-count=1 >> ..\colony.core\build_number_%BLDNUM%.txt
popd

:: Tag the Build (note: Even failed builds will be tagged - this is the lesser of the evils because if I tag at the end (builds take awhile) - the tag will fail because my snapshot is behead the head)
IF "/%1"=="/" goto :beginbuild
set TAG=%PKG_BRANCH%-bldnum-%BLDNUM%
echo:build tag: %TAG% >> build_number_%BLDNUM%.txt
pushd colony.core
set HELPER=%TOP_DIR%git-credential-helper.bat
set GIT_ASKPASS=%HELPER%
git tag %TAG%
IF NOT ERRORLEVEL 1 goto :tagready
git tag -d %TAG%
git push --delete origin %TAG%
git tag %TAG%
:tagready
git push origin HEAD:%PKG_BRANCH% %TAG%
IF ERRORLEVEL 1 exit /b 1
popd

:: Use Outcast's publish script to build and test everything (but do NOT actually publish)
:beginbuild
orc mkwrk
pushd colony.core
call  setnqbp.bat
@echo on
orc -v publish --ci %WORKSPACE%\colony.core\top\ci_local_packages.bat --version 0.0.0 colony.core "dummy comment - not really publishing" --dry-run --nonewer --nopending --nons --keeptars --nodeps --nogetdeps --noclean
IF ERRORLEVEL 1 exit /b 1

:: Run doxygen
pushd top
run_doxygen.py
IF ERRORLEVEL 1 exit /b 1
popd

:: Generate code coverage reports (clean the linux projects as to not confuse the gcovr files)
pushd tests
bob.py build linux -z
chuck.py -v --match tca.py args rpt --xml-pretty -o gcovr.xml
IF ERRORLEVEL 1 exit /b 1
popd

