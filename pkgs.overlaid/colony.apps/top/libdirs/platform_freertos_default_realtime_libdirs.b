# This libdirs.b snippet provides the default platform, trace, time, etc.
# support for a FreeRTOS project using 'realtime' system ticks.
 

# FreeRTOS specific Colony.core
src/Cpl/System/FreeRTOS
src/Cpl/System/FreeRTOS/_fatalerror

# supporting infrastructure
src/Cpl/System
src/Cpl/System/_trace
src/Cpl/Container
src/Cpl/Text
src/Cpl/Io

# NOTE: Your App still needs:
#       1) BSP directories
#       2) The target/compiler specific libdirs.b for the actual FreeRTOS code
#       3) Driver, stdio, serial port, etc. support for the Cpl::System::Trace output
#
# Below is example of the above list:

# BSP
#pkg.overlaid/colony.bsp.renesas.rx/top/libdirs/yrdkr62n_gnurx_libdirs.b

# FreeRTOS
#pkg.overload/freertos/top/libdirs/gcc_rx600_libdirs.b

# drivers for trace output
#src/Cpl/Io/Serial/Renesas/Rx62n
#src/Driver/Uart/Stream



