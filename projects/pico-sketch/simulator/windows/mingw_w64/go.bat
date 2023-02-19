@echo off
:: 
:: Helper script that launch the Pico-Display-Simulator, then waits for display
:: simulator to start, then launch the simulated application.
::
:: Assume that the projects/pico-display-simulation MS solution has built a
:: 'release' build
::
echo:Launching display simulator...
start ..\..\..\..\pico-display-simulator\simulator\bin\Release\simulator.exe
timeout /t 1
_win32\pico-sketch.exe
