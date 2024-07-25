@echo off
:: Helper script that cleans up the previous RATT test before
:: launching a new executing of the RATT test.
::
:: NOTE: Clean-up is important especially in the case of FAILED ratt test

taskkill /F /IM b.exe
del _win32\ratt*
cls
a.py
