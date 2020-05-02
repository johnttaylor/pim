@echo off
:: Script to access the git username/password via Environment variables
set arg=%~1
IF "%arg:~0,4%"=="Pass" echo:%GITHUB_PASSWORD%
IF "%arg:~0,4%"=="User" echo:%GITHUB_USER%

