# Common stuff
../../../libdirs.b

# Board specific directories....
src/Storm/Thermostat/Main/_adafruit_grand_central_m4
src/Storm/Thermostat/SimHouse

# Board specific middle-ware
//colony.core/src/Cpl/TShell/Cmd/FreeRTOS
//colony.core/src/Cpl/System/FreeRTOS
//colony.core/src/Cpl/System/FreeRTOS/_fatalerror
//colony.core/src/Cpl/System/_trace
//colony.arduino/src/Cpl/System/_trace/_arduino
//colony.arduino/src/Cpl/Io/Serial/Arduino
//colony.arduino/src/Bsp/Adafruit/grand_central_m4/gcc
//colony.arduino/src/Bsp/Adafruit/grand_central_m4/gcc/FreeRTOS/source
//colony.arduino/src/Bsp/Adafruit/grand_central_m4/gcc/FreeRTOS/source/portable/GCC/ARM_CM4F  
//colony.arduino/src/Bsp/Adafruit/grand_central_m4/gcc/FreeRTOS/source/portable/MemMang  

//colony.core/src/Cpl/Io/Stdio
//colony.core/src/Cpl/Io/File
//colony.arduino/src/Cpl/Io/File/Arduino/_sdfat
//colony.arduino/src/Cpl/Io/File/Arduino/_sdfat/_api

# Arduino Core library for the Adafruit's Grand Central M4 board (Atmel SAMD51 mcu)
//colony.arduino/arduino/hardware/samd/$ARDUINO_BSP_VER$/cores/arduino/USB
//colony.arduino/arduino/hardware/samd/$ARDUINO_BSP_VER$/libraries/Wire
//colony.arduino/arduino/hardware/samd/$ARDUINO_BSP_VER$/variants/grand_central_m4
//colony.arduino/arduino/hardware/samd/$ARDUINO_BSP_VER$/cores/arduino > main.cpp cortex_handlers.c wiring.c

# NeoPixe library
//colony.arduino/arduino/libraries/Adafruit_NeoPixel

# SDFAT
//colony.arduino/arduino/libraries/SdFat/src
//colony.arduino/arduino/libraries/SdFat/src/FatLib
//colony.arduino/arduino/libraries/Adafruit_SPIFlash/src
//colony.arduino/arduino/libraries/Adafruit_SPIFlash/src/qspi < Adafruit_FlashTransport_QSPI_SAMD.cpp
//colony.arduino/arduino/libraries/Adafruit_Zero_DMA_Library
