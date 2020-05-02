# Libraries
xsrc/colony.arduino/arduino/libraries/SdFat/src
xsrc/colony.arduino/arduino/libraries/SdFat/src/FatLib
xsrc/colony.arduino/arduino/libraries/Adafruit_SPIFlash/src
xsrc/colony.arduino/arduino/libraries/Adafruit_SPIFlash/src/qspi < Adafruit_FlashTransport_QSPI_SAMD.cpp
xsrc/colony.arduino/arduino/libraries/Adafruit_Zero_DMA_Library

# Colony Core infrastructure
#//colony.core/src/Cpl/Text/Frame
#//colony.core/src/Cpl/Text/Tokenizer
#//colony.core/src/Cpl/Memory
src/Cpl/Container
#//colony.core/src/Cpl/Checksum
src/Cpl/Text
src/Cpl/Io
src/Cpl/Dm
#//colony.core/src/Cpl/Dm/Mp
#//colony.core/src/Cpl/Dm/TShell
src/Cpl/Itc
src/Cpl/System

# Board specific middle-ware
src/Cpl/TShell/Cmd/FreeRTOS
src/Cpl/System/FreeRTOS
src/Cpl/System/FreeRTOS/_fatalerror
src/Cpl/System/_trace
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


