# Libraries
xsrc/colony.arduino/arduino/libraries/SdFat/src
xsrc/colony.arduino/arduino/libraries/SdFat/src/FatLib
xsrc/colony.arduino/arduino/libraries/Adafruit_SPIFlash/src
xsrc/colony.arduino/arduino/libraries/Adafruit_SPIFlash/src/qspi < Adafruit_FlashTransport_QSPI_SAMD.cpp
xsrc/colony.arduino/arduino/libraries/Adafruit_Zero_DMA_Library

# Unit under test
#src/Cpl/Io/File/_ardunio/_sdfat
#src/Cpl/Io/File/_ardunio/_sdfat/_api
//colony.core/src/Cpl/Io/File

# tests
#//colony.core/src/Cpl/Io/File/_0test


# Colony Core infrastructure
#//colony.core/src/Cpl/Text/Frame
#//colony.core/src/Cpl/Text/Tokenizer
#//colony.core/src/Cpl/Memory
//colony.core/src/Cpl/Container
#//colony.core/src/Cpl/Checksum
//colony.core/src/Cpl/Text
//colony.core/src/Cpl/Io
//colony.core/src/Cpl/Dm
#//colony.core/src/Cpl/Dm/Mp
#//colony.core/src/Cpl/Dm/TShell
//colony.core/src/Cpl/Itc
//colony.core/src/Cpl/System

# Board specific middle-ware
//colony.core/src/Cpl/TShell/Cmd/FreeRTOS
//colony.core/src/Cpl/System/FreeRTOS
//colony.core/src/Cpl/System/FreeRTOS/_fatalerror
//colony.core/src/Cpl/System/_trace
src/Cpl/System/_trace/_arduino
src/Cpl/Io/Serial/Arduino
src/Bsp/Adafruit/grand_central_m4/gcc
src/Bsp/Adafruit/grand_central_m4/gcc/FreeRTOS/source
src/Bsp/Adafruit/grand_central_m4/gcc/FreeRTOS/source/portable/GCC/ARM_CM4F  
src/Bsp/Adafruit/grand_central_m4/gcc/FreeRTOS/source/portable/MemMang  

# Arduino Core library for the Adafruit's Grand Central M4 board (Atmel SAMD51 mcu)
xsrc/colony.arduino/arduino/hardware/samd/$ARDUINO_BSP_VER$/cores/arduino/USB
xsrc/colony.arduino/arduino/hardware/samd/$ARDUINO_BSP_VER$/libraries/Wire
xsrc/colony.arduino/arduino/hardware/samd/$ARDUINO_BSP_VER$/variants/grand_central_m4
xsrc/colony.arduino/arduino/hardware/samd/$ARDUINO_BSP_VER$/cores/arduino > main.cpp cortex_handlers.c wiring.c


