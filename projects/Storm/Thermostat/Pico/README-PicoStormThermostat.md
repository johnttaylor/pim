# Pico Storm Thermostat Application.

This page assumes that you have read the [detailed introduction](https://github.com/johnttaylor/colony.pico/blob/main/projects/Storm/Thermostat/Pico/README.md) to the Pico Storm Thermostat

The project build directory for the Pico hardware is located at:
>`projects\Storm\Pico\rp2040\pico-uart\windows\gcc`

The project build directory for the Windoze simluator is located at:
>`projects\Storm\Pico\simulator\windows\mingw_w64`

The project build directories for the 'original' simulator are located at:
  
>```
>projects\Storm\simulation\linux\gcc
>projects\Storm\simulation\windows\vc12
>projects\Storm\simulation\windows\mingw_w64
>```


#### Features, Constraints, etc.
- A primitive UI that allows the user to set the thermostat mode, fan operating mode, and the set the cooling and heating setpoints.
  - Indoor temperature is displayed
  - Outdoor temperature is displayed - when it is available
  - The HVAC output relays/signals are displayed when they are actively on
  - The RGB Led indicates the thermostat's operating mode (blue:=cooling, red:=heating)
- A debug Console with a full set of commands that allow the user to fully exercise, monitor, and test the application.
- A functional simulation (including the UI) is provided that allows the user to run the application on Windows PC.  
- My Pico platform does not support relays, temperature sensors, or persistent storage:
  - To mitigate the lack of sensors/relay outputs - a very crude house simulation is included in the application that provides a closed-loop feedback based on outdoor temperature, indoor temperature, and requested active HVAC capacity.
  - When running on the physical hardware no persistent storage is available, i.e. all configuration and user settings are defaulted on power-up
    - The functional simulator *does* support persistent storage.

#### UI
The UI hardware is a Pimoroni Pico Display board that contains a 240 x 135 Graphic LCD with 4 momentary push buttons and one RGB LED
   
##### Keys
- `A` button - Toggles the Thermostat mode settings
- `B` button - Toggles the Fan Setting
- `X` button - increments the set point
- `Y` button - decrements the set point
    - The initial setpoint that is edited is the Cooling setpoint.  Pressing the `A` or `B` button while the setpoint is *highlighted* will toggle between the Cooling and Heating setpoints.

**Note:** The user settings are *highlighted* when they are being edited.  There is 5 second delay once the user has finished their changes before the setting is un-highligthed and the system is updated with the new value(s). 

#### Debug Console
The Debug Console on the physical target used the Pico's `UART0`.  On the simulator, the terminal window of where the simulation was launched serves as the debug console.

Typing `help` or `help *` in the Console window will display a list of available commands

The following set of console commands can be used to exercise the thermostat, i.e. copy-paste the command into the Console window:
- Cooling operation
  >```
  >state
  >house enable 99
  >user setpt 70 60
  >user mode eAUTO
  >user fan eAUTO
  >state
  >wb enot disable
  >state
  >```

- Heating operation
  >```
  >state
  >house enable -20
  >user setpt 80 70
  >user mode eAUTO
  >user fan eAUTO
  >state
  >wb enot disable
  >state
  >```
- Change the HVAC Equipment configuration (by writing to a model point).  The following command configures the system to: Three Stage furnace with a variable speed blower motor and single stage Air Conditioner
  >`dm write {name:"equipmentConfig",val: {iduType:"eFURNACE",vspBlower:true,numIduHeat:3,oduType:"eAC",numCompStages:1}}
`